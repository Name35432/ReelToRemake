// Fill out your copyright notice in the Description page of Project Settings.


#include "FileLoader.h"
#include "HAL/PlatformProcess.h"
#include "Misc/Paths.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/FileHelper.h"
#include "Serialization/MemoryReader.h"
#include <XmlFile.h>
#include "Serialization/BufferArchive.h"

int32 FileLoader::Read7BitEncodedInt(FMemoryReader& Ar)
{
    int32 Count = 0;
    int32 Shift = 0;
    uint8 ByteVal = 0;

    do
    {
        if (Ar.AtEnd() || Shift == 5 * 7)
        {
            return 0;
        }

        Ar << ByteVal;
        Count |= (ByteVal & 0x7F) << Shift;
        Shift += 7;
    } while ((ByteVal & 0x80) != 0);

    return Count;
}

void FileLoader::Write7BitEncodedInt(FBufferArchive& Ar, int32 Value)
{
    uint32 UnsignedValue = (uint32)Value;
    while (UnsignedValue >= 0x80)
    {
        uint8 ByteVal = (uint8)(UnsignedValue | 0x80);
        Ar << ByteVal;
        UnsignedValue >>= 7;
    }
    uint8 FinalByte = (uint8)UnsignedValue;
    Ar << FinalByte;
}

bool FileLoader::ParseJaggedJsonArray(const FString& JsonString, TArray<TArray<int32>>& OutArray)
{
    OutArray.Reset();

    if (JsonString.IsEmpty())
    {
        return false;
    }

    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<TCHAR>::Create(JsonString);
    TArray<TSharedPtr<FJsonValue>> OuterJsonArray;

    if (!FJsonSerializer::Deserialize(Reader, OuterJsonArray))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to deserialize JSON string."));
        return false;
    }

    OutArray.Reserve(OuterJsonArray.Num());

    for (const TSharedPtr<FJsonValue>& OuterVal : OuterJsonArray)
    {
        TArray<int32> InnerArray;
        const TArray<TSharedPtr<FJsonValue>>* InnerJsonArray = nullptr;

        if (OuterVal->TryGetArray(InnerJsonArray) && InnerJsonArray)
        {
            InnerArray.Reserve(InnerJsonArray->Num());
            for (const TSharedPtr<FJsonValue>& InnerVal : *InnerJsonArray)
            {
                InnerArray.Add(FMath::FloorToInt(InnerVal->AsNumber()));
            }
        }

        OutArray.Add(MoveTemp(InnerArray));
    }

    return true;
}

void FileLoader::LoadRshw(const FString& FilePath, TArray<BitMap>& ShowData, TArray<uint8>& AudioData)
{
    FString ExePath = FPaths::Combine(FPaths::ProjectDir(), TEXT("Binaries/Win64/UnrealRSHW.exe"));

    FString TempDir = FPaths::ProjectSavedDir() / TEXT("TempRSHW");
    IFileManager::Get().MakeDirectory(*TempDir, true);

    FString TempAudioPath = TempDir / TEXT("temp_audio.wav");
    FString TempSignalsPath = TempDir / TEXT("temp_signals.txt");

    IFileManager::Get().Delete(*TempAudioPath);
    IFileManager::Get().Delete(*TempSignalsPath);

    FString CommandArgs = FString::Printf(TEXT("\"%s\" \"%s\" \"%s\""),
        *FPaths::ConvertRelativePathToFull(FilePath),
        *FPaths::ConvertRelativePathToFull(TempAudioPath),
        *FPaths::ConvertRelativePathToFull(TempSignalsPath)
    );

    int32 ReturnCode = -1;
    FProcHandle ProcHandle = FPlatformProcess::CreateProc(
        *ExePath,
        *CommandArgs,
        true,
        true,
        true,
        nullptr,
        0,
        nullptr,
        nullptr
    );

    if (!ProcHandle.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("[RSHW Loader] Failed to launch executable at %s"), *ExePath);
        return;
    }

    FPlatformProcess::WaitForProc(ProcHandle);
    FPlatformProcess::GetProcReturnCode(ProcHandle, &ReturnCode);
    FPlatformProcess::CloseProc(ProcHandle);

    if (ReturnCode != 0)
    {
        UE_LOG(LogTemp, Error, TEXT("[RSHW Loader] Executable exited with code %d"), ReturnCode);
        return;
    }

    if (FPaths::FileExists(TempAudioPath))
    {
        FFileHelper::LoadFileToArray(AudioData, *TempAudioPath);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[RSHW Loader] Temp audio file missing."));
    }

    if (FPaths::FileExists(TempSignalsPath))
    {
        TArray<FString> SignalLines;
        if (FFileHelper::LoadFileToStringArray(SignalLines, *TempSignalsPath))
        {
            ShowData.Empty();
            ShowData.Reserve(SignalLines.Num());

            for (const FString& Line : SignalLines)
            {
                BitMap FrameMap;

                for (int32 Index = 0; Index < Line.Len() && Index < 300; ++Index)
                {
                    FrameMap.SetContinousData(Index, (Line[Index] == '1'));
                }

                ShowData.Add(FrameMap);
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[RSHW Loader] Temp signals file missing."));
    }

    IFileManager::Get().Delete(*TempAudioPath);
    IFileManager::Get().Delete(*TempSignalsPath);
}

void FileLoader::LoadRtr(const FString& FilePath, TArray<BitMap>& ShowData, TArray<uint8>& AudioData)
{
    TArray<uint8> FileBuffer;
    if (!FFileHelper::LoadFileToArray(FileBuffer, *FilePath))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to read file from disk: %s"), *FilePath);
        return;
    }

    FMemoryReader Ar(FileBuffer, true);

    const FString ExpectedHeader = TEXT("Reel To Remake Show File (RTRSF)");
    int32 HeaderLen = ExpectedHeader.Len();

    if (Ar.TotalSize() < HeaderLen) return;

    TArray<uint8> HeaderBytes;
    HeaderBytes.AddUninitialized(HeaderLen);
    Ar.Serialize(HeaderBytes.GetData(), HeaderLen);

    FString HeaderStr = FString(HeaderLen, (ANSICHAR*)HeaderBytes.GetData());
    if (HeaderStr != ExpectedHeader)
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid header in file: %s"), *FilePath);
        return;
    }

    // Skip 15 bytes after header
    Ar.Seek(Ar.Tell() + 15);

    // Read Audio Data
    int32 AudioLength = 0;
    uint8 DummyByte = 0;

    Ar << AudioLength;
    Ar << DummyByte;

    if (AudioLength > 0)
    {
        AudioData.Reset(AudioLength);
        AudioData.AddUninitialized(AudioLength);
        Ar.Serialize(AudioData.GetData(), AudioLength);
    }
    else
    {
        AudioData.Reset();
    }

    // Skip 15 bytes
    Ar.Seek(Ar.Tell() + 15);

    // Read Frame Data
    int32 FrameCount = 0;
    Ar << FrameCount;

    ShowData.Reset();
    ShowData.SetNum(FrameCount);

    for (int32 f = 0; f < FrameCount; ++f)
    {
        int32 Count = 0;

        // TopDrawer (byte[])
        Ar << Count;
        Ar << DummyByte;
        if (Count > 0)
        {
            ShowData[f].TopDrawer.reserve(Count);
            Ar.Serialize(ShowData[f].TopDrawer.data(), Count);
        }
        Ar.Seek(Ar.Tell() + 5);

        // BottomDrawer (byte[])
        Ar << Count;
        Ar << DummyByte;
        if (Count > 0)
        {
            ShowData[f].BottomDrawer.reserve(Count);
            Ar.Serialize(ShowData[f].BottomDrawer.data(), Count);
        }
        Ar.Seek(Ar.Tell() + 5);

        // DmxData (int32[])
        Ar << Count;
        Ar << DummyByte;
        if (Count > 0)
        {
            ShowData[f].DmxData.reserve(Count);
            for (int32 i = 0; i < Count; ++i)
            {
                int32 Val = 0;
                Ar << Val; // Explicitly serialize 32-bit int
                ShowData[f].DmxData[i] = Val;
            }
        }
        Ar.Seek(Ar.Tell() + 5);

        // AvData (int32[])
        Ar << Count;
        Ar << DummyByte;
        if (Count > 0)
        {
            ShowData[f].AvData.reserve(Count);
            for (int32 i = 0; i < Count; ++i)
            {
                int32 Val = 0;
                Ar << Val; // Explicitly serialize 32-bit int
                ShowData[f].AvData[i] = Val;
            }
        }
        Ar.Seek(Ar.Tell() + 5);
    }

    // Skip 15 bytes
    Ar.Seek(Ar.Tell() + 15);

    // Read XML string length (7-bit encoded int in .NET)
    int32 XmlStringLen = Read7BitEncodedInt(Ar);

    FString OutTimeSaved;
    FString OutSavedBy;

    if (XmlStringLen > 0 && (Ar.Tell() + XmlStringLen <= Ar.TotalSize()))
    {
        TArray<uint8> XmlBytes;
        XmlBytes.AddUninitialized(XmlStringLen);
        Ar.Serialize(XmlBytes.GetData(), XmlStringLen);

        FString XmlString = FString(XmlStringLen, (ANSICHAR*)XmlBytes.GetData());

        // Construct from Text string buffer
        FXmlFile XmlFile(XmlString, EConstructMethod::ConstructFromBuffer);
        if (XmlFile.IsValid())
        {
            FXmlNode* RootNode = XmlFile.GetRootNode();
            if (RootNode)
            {
                FXmlNode* TimeNode = RootNode->FindChildNode(TEXT("TimeSaved"));
                if (TimeNode) OutTimeSaved = TimeNode->GetContent();

                FXmlNode* SavedByNode = RootNode->FindChildNode(TEXT("SavedBy"));
                if (SavedByNode) OutSavedBy = SavedByNode->GetContent();
            }
        }
    }

    UE_LOG(LogTemp, Log, TEXT("Successfully loaded %s! Last Saved: %s By: %s"),
        *FPaths::GetCleanFilename(FilePath), *OutTimeSaved, *OutSavedBy);
}

void FileLoader::LoadShow(const FString& FilePath, TArray<BitMap>& ShowData, TArray<uint8>& AudioData)
{
    FString Ext = FPaths::GetExtension(FilePath, true);
    if (Ext != ".rtr")
    {
        LoadRshw(FilePath, ShowData, AudioData);
    }
    else
    {
        LoadRtr(FilePath, ShowData, AudioData);
    }
}

void FileLoader::SaveShow(const FString& FilePath, TArray<BitMap>& ShowData, TArray<uint8>& AudioData)
{
    FBufferArchive Ar;

    FString HeaderStr = TEXT("Reel To Remake Show File (RTRSF)");
    FTCHARToUTF8 HeaderUtf8(*HeaderStr);
    Ar.Serialize((void*)HeaderUtf8.Get(), HeaderUtf8.Length());

    uint8 Padding15[15] = { 0 };
    Ar.Serialize(Padding15, 15);

    int32 AudioLength = AudioData.Num();
    Ar << AudioLength;

    uint8 ZeroByte = 0;
    Ar << ZeroByte;

    if (AudioLength > 0)
    {
        Ar.Serialize((void*)AudioData.GetData(), AudioLength);
    }

    Ar.Serialize(Padding15, 15);

    int32 FrameCount = ShowData.Num();
    Ar << FrameCount;

    uint8 Padding5[5] = { 0 };

    for (const BitMap& Frame : ShowData)
    {
        int32 TopLen = 19;
        Ar << TopLen << ZeroByte;
        if (TopLen > 0)
        {
            Ar.Serialize((void*)Frame.TopDrawer.data(), TopLen);
        }
        Ar.Serialize(Padding5, 5);

        int32 BottomLen = 19;
        Ar << BottomLen << ZeroByte;
        if (BottomLen > 0)
        {
            Ar.Serialize((void*)Frame.BottomDrawer.data(), BottomLen);
        }
        Ar.Serialize(Padding5, 5);

        int32 DmxLen = 512;
        Ar << DmxLen << ZeroByte;
        for (int32 Value : Frame.DmxData)
        {
            Ar << Value;
        }
        Ar.Serialize(Padding5, 5);

        int32 AvLen = 40;
        Ar << AvLen << ZeroByte;
        for (int32 Value : Frame.AvData)
        {
            Ar << Value;
        }
        Ar.Serialize(Padding5, 5);
    }

    Ar.Serialize(Padding15, 15);

    FString XmlContent = FString::Printf(
        TEXT("<Root><TimeSaved>%s</TimeSaved><SavedBy>%s</SavedBy></Root>"),
        *FDateTime::Now().ToString(TEXT("%Y-%m-%d %H:%M:%S")),
        FPlatformProcess::UserName()
    );

    FTCHARToUTF8 XmlUtf8(*XmlContent);
    int32 XmlByteLen = XmlUtf8.Length();

    Write7BitEncodedInt(Ar, XmlByteLen);
    Ar.Serialize((void*)XmlUtf8.Get(), XmlByteLen);

    bool bSuccess = FFileHelper::SaveArrayToFile(Ar, *FilePath);
    if (bSuccess)
    {
        UE_LOG(LogTemp, Log, TEXT("Saved show file successfully to %s"), *FilePath);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to save show file to %s"), *FilePath);
    }
}