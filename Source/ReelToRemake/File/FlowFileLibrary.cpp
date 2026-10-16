// These are Reel To Remake assets. If you are seeing this, you are in big trouble pal!


#include "FlowFileLibrary.h"

#include "HAL/PlatformFileManager.h"
#include "Misc/FileHelper.h"

namespace FlowFileDecoder
{
    static constexpr TCHAR Header[] =
        TEXT("Reel To Remake Flow File Format (RTRFFF)");

    static constexpr int32 PaddingSize = 15;
    static constexpr uint32 MaxStringBytes = 64 * 1024 * 1024;
    static constexpr int32 MaxFlows = 100000;
    static constexpr int32 MaxArrayElements = 1000000;

    namespace
    {
        class FReader
        {
        public:
            explicit FReader(const TArray<uint8>& InData)
                : Data(InData)
            {
            }

            int64 Position = 0;

            bool ReadByte(uint8& Out)
            {
                if (Position < 0 || Position >= Data.Num())
                    return false;

                Out = Data[Position++];
                return true;
            }

            bool Skip(int64 Count)
            {
                if (Count < 0 ||
                    Position < 0 ||
                    Count > Data.Num() - Position)
                {
                    return false;
                }

                Position += Count;
                return true;
            }

            bool ReadInt32(int32& Out)
            {
                uint32 Bits;

                if (!ReadUInt32(Bits))
                    return false;

                FMemory::Memcpy(&Out, &Bits, sizeof(Out));
                return true;
            }

            bool ReadUInt32(uint32& Out)
            {
                if (Position < 0 ||
                    Data.Num() - Position < 4)
                {
                    return false;
                }
            
                Out =
                    static_cast<uint32>(Data[Position]) |
                    (static_cast<uint32>(Data[Position + 1]) << 8) |
                    (static_cast<uint32>(Data[Position + 2]) << 16) |
                    (static_cast<uint32>(Data[Position + 3]) << 24);

                Position += 4;
                return true;
            }

            bool ReadFloat(float& Out)
            {
                uint32 Bits;

                if (!ReadUInt32(Bits))
                    return false;
            
                FMemory::Memcpy(&Out, &Bits, sizeof(Out));
                return true;
            }

            bool ReadDotNetString(FString& Out)
            {
                uint32 ByteCount = 0;
                uint32 Shift = 0;

                for (int32 i = 0; i < 5; ++i)
                {
                    uint8 Byte;

                    if (!ReadByte(Byte))
                        return false;

                    if (i == 4 && (Byte & 0xF0) != 0)
                        return false;

                    ByteCount |=
                        static_cast<uint32>(Byte & 0x7F) << Shift;

                    if ((Byte & 0x80) == 0)
                    {
                        if (ByteCount > MaxStringBytes ||
                            ByteCount > static_cast<uint32>(
                                Data.Num() - Position))
                        {
                            return false;
                        }

                        FUTF8ToTCHAR Converted(
                            reinterpret_cast<const ANSICHAR*>(
                                Data.GetData() + Position),
                            static_cast<int32>(ByteCount));

                        Out = FString(
                            Converted.Length(),
                            Converted.Get());

                        Position += ByteCount;
                        return true;
                    }

                    Shift += 7;
                }

                return false;
            }

            bool ReadSeparator()
            {
                uint8 Byte;
                return ReadByte(Byte) && Byte == 0;
            }

            bool ReadFloatArray(TArray<float>& Out)
            {
                int32 Count;

                if (!ReadInt32(Count) ||
                    Count < 0 ||
                    Count > MaxArrayElements)
                {
                    return false;
                }
            
                if (static_cast<int64>(Count) * 4 >
                    Data.Num() - Position)
                {
                    return false;
                }

                Out.SetNumUninitialized(Count);

                for (int32 i = 0; i < Count; ++i)
                {
                    if (!ReadFloat(Out[i]))
                        return false;
                }

                return true;
            }

        private:
            const TArray<uint8>& Data;
        };
    }
    
        namespace
        {
            class FWriter
            {
            public:
                TArray<uint8> Data;

                void WriteByte(uint8 Value)
                {
                    Data.Add(Value);
                }

                void WritePadding(int32 Count)
                {
                    for (int32 i = 0; i < Count; ++i)
                    {
                        Data.Add(0);
                    }
                }

                void WriteInt32(int32 Value)
                {
                    uint32 Bits;
                    FMemory::Memcpy(&Bits, &Value, sizeof(Bits));

                    WriteUInt32(Bits);
                }

                void WriteUInt32(uint32 Value)
                {
                    Data.Add(static_cast<uint8>(Value));
                    Data.Add(static_cast<uint8>(Value >> 8));
                    Data.Add(static_cast<uint8>(Value >> 16));
                    Data.Add(static_cast<uint8>(Value >> 24));
                }

                void WriteFloat(float Value)
                {
                    uint32 Bits;
                    FMemory::Memcpy(&Bits, &Value, sizeof(Bits));

                    WriteUInt32(Bits);
                }

                void WriteDotNetString(const FString& Value)
                {
                    // BinaryWriter.Write(string) encodes strings as UTF-8
                    // with a 7-bit encoded byte length prefix.
                    FTCHARToUTF8 Converted(*Value);

                    const int32 ByteCount = Converted.Length();

                    uint32 Length = static_cast<uint32>(ByteCount);

                    while (Length >= 0x80)
                    {
                        WriteByte(
                            static_cast<uint8>((Length & 0x7F) | 0x80));

                        Length >>= 7;
                    }

                    WriteByte(static_cast<uint8>(Length));

                    if (ByteCount > 0)
                    {
                        const int32 Offset = Data.Num();
                        Data.AddUninitialized(ByteCount);

                        FMemory::Memcpy(
                            Data.GetData() + Offset,
                            Converted.Get(),
                            ByteCount);
                    }
                }

                void WriteSeparator()
                {
                    WriteByte(0);
                }

                void WriteFloatArray(const TArray<float>& Values)
                {
                    WriteInt32(Values.Num());

                    for (const float Value : Values)
                    {
                        WriteFloat(Value);
                    }
                }
            };
        }
    
    static FString EscapeXml(const FString& Input)
    {
        FString Result = Input;

        Result.ReplaceInline(
            TEXT("&"), TEXT("&amp;"), ESearchCase::CaseSensitive);

        Result.ReplaceInline(
            TEXT("<"), TEXT("&lt;"), ESearchCase::CaseSensitive);

        Result.ReplaceInline(
            TEXT(">"), TEXT("&gt;"), ESearchCase::CaseSensitive);

        Result.ReplaceInline(
            TEXT("\""), TEXT("&quot;"), ESearchCase::CaseSensitive);

        Result.ReplaceInline(
            TEXT("'"), TEXT("&apos;"), ESearchCase::CaseSensitive);

        return Result;
    }
}

bool UFlowFileLibrary::LoadFlowFile(const FString& FilePath, TArray<FFlowAdjustment>& OutFlows, FString& OutMetadataXml, FString& OutError)
{
    using namespace FlowFileDecoder;

    OutFlows.Reset();
    OutMetadataXml.Empty();
    OutError.Empty();

    TArray<uint8> FileData;

    if (!FFileHelper::LoadFileToArray(FileData, *FilePath))
    {
        OutError = FString::Printf(
            TEXT("Could not read flow file: %s"),
            *FilePath);

        return false;
    }

    FReader Reader(FileData);
    
    FString FileHeader;

    if (!Reader.ReadDotNetString(FileHeader) ||
        FileHeader != Header)
    {
        OutError = TEXT("Invalid flow file header.");
        return false;
    }

    if (!Reader.Skip(PaddingSize))
    {
        OutError = TEXT("File ended inside header padding.");
        return false;
    }

    int32 FlowCount = 0;

    if (!Reader.ReadInt32(FlowCount) ||
        FlowCount < 0 ||
        FlowCount > MaxFlows)
    {
        OutError = TEXT("Invalid flow count.");
        return false;
    }

    if (!Reader.ReadSeparator())
    {
        OutError = TEXT("Missing flow-count separator.");
        return false;
    }

    OutFlows.Reserve(FlowCount);

    for (int32 Index = 0; Index < FlowCount; ++Index)
    {
        FFlowAdjustment Flow;

        if (!Reader.ReadDotNetString(Flow.CharName))
        {
            OutError = FString::Printf(
                TEXT("Could not read character name at flow %d."),
                Index);

            OutFlows.Reset();
            return false;
        }

        if (!Reader.ReadSeparator() ||
            !Reader.ReadFloatArray(Flow.SpeedIn) ||
            !Reader.ReadSeparator() ||
            !Reader.ReadFloatArray(Flow.SpeedOut) ||
            !Reader.ReadSeparator() ||
            !Reader.ReadFloatArray(Flow.AccelIn) ||
            !Reader.ReadSeparator() ||
            !Reader.ReadFloatArray(Flow.AccelOut) ||
            !Reader.ReadSeparator() ||
            !Reader.ReadFloatArray(Flow.MovementDelayIn) ||
            !Reader.ReadSeparator() ||
            !Reader.ReadFloatArray(Flow.MovementDelayOut) ||
            !Reader.ReadSeparator())
        {
            OutError = FString::Printf(
                TEXT("Invalid or truncated data in flow %d."),
                Index);

            OutFlows.Reset();
            return false;
        }

        OutFlows.Add(MoveTemp(Flow));
    }
    
    if (!Reader.Skip(PaddingSize))
    {
        OutError = TEXT("File ended inside trailing padding.");
        OutFlows.Reset();
        return false;
    }
    
    if (!Reader.ReadDotNetString(OutMetadataXml))
    {
        OutError = TEXT("Could not read trailing XML metadata.");
        OutFlows.Reset();
        return false;
    }

    return true;
}

bool UFlowFileLibrary::SaveFlowFile(const FString& FilePath, const TArray<FFlowAdjustment>& InFlows, const FString& SavedBy, FString& OutError)
{
    using namespace FlowFileDecoder;

    OutError.Empty();

    if (FilePath.IsEmpty())
    {
        OutError = TEXT("The flow file path is empty.");
        return false;
    }

    if (InFlows.Num() > MaxFlows)
    {
        OutError = TEXT("The flow count exceeds the supported limit.");
        return false;
    }
    
    FWriter Writer;
    
    Writer.WriteDotNetString(Header);
    Writer.WritePadding(PaddingSize);
    
    Writer.WriteInt32(InFlows.Num());
    Writer.WriteSeparator();
    
    for (int32 Index = 0; Index < InFlows.Num(); ++Index)
    {
        const FFlowAdjustment& Flow = InFlows[Index];

        const TArray<float>* Arrays[] =
        {
            &Flow.SpeedIn,
            &Flow.SpeedOut,
            &Flow.AccelIn,
            &Flow.AccelOut,
            &Flow.MovementDelayIn,
            &Flow.MovementDelayOut
        };

        for (const TArray<float>* Array : Arrays)
        {
            if (Array->Num() > MaxArrayElements)
            {
                OutError = FString::Printf(
                    TEXT("Flow %d contains an oversized float array."),
                    Index);

                return false;
            }
        }
        
        Writer.WriteDotNetString(Flow.CharName);
        Writer.WriteSeparator();
        
        Writer.WriteFloatArray(Flow.SpeedIn);
        Writer.WriteSeparator();
        
        Writer.WriteFloatArray(Flow.SpeedOut);
        Writer.WriteSeparator();

        Writer.WriteFloatArray(Flow.AccelIn);
        Writer.WriteSeparator();

        Writer.WriteFloatArray(Flow.AccelOut);
        Writer.WriteSeparator();
        
        Writer.WriteFloatArray(Flow.MovementDelayIn);
        Writer.WriteSeparator();
        
        Writer.WriteFloatArray(Flow.MovementDelayOut);
        Writer.WriteSeparator();
    }
    
    Writer.WritePadding(PaddingSize);
    
    const FString TimeSaved =
        FDateTime::Now().ToString(TEXT("%Y-%m-%d %H:%M:%S"));

    const FString MetadataXml = FString::Printf(
        TEXT("<Root>\n")
        TEXT("  <TimeSaved>%s</TimeSaved>\n")
        TEXT("  <SavedBy>%s</SavedBy>\n")
        TEXT("</Root>"),
        *EscapeXml(TimeSaved),
        *EscapeXml(SavedBy));
    
    Writer.WriteDotNetString(MetadataXml);
    
    const FString Directory = FPaths::GetPath(FilePath);

    if (!Directory.IsEmpty() &&
        !IFileManager::Get().MakeDirectory(*Directory, true))
    {
        OutError = FString::Printf(
            TEXT("Could not create directory: %s"),
            *Directory);

        return false;
    }
    
    if (!FFileHelper::SaveArrayToFile(Writer.Data, *FilePath))
    {
        OutError = FString::Printf(
            TEXT("Could not save flow file: %s"),
            *FilePath);

        return false;
    }

    return true;
}