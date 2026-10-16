#include "AudioParser.h"
#include "../Thirdparty/miniaudio.h"
#include "Serialization/BulkData.h"
#include "Memory/SharedBuffer.h"
#include "HAL/PlatformFileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/PlatformProcess.h"
#include "Sound/SoundWaveProcedural.h"

#pragma pack(push, 1)

struct FWAVHeader
{
    char RIFF[4] = { 'R', 'I', 'F', 'F' };
    uint32 ChunkSize = 0;

    char WAVE[4] = { 'W', 'A', 'V', 'E' };
    char Fmt[4] = { 'f', 'm', 't', ' ' };
    uint32 Subchunk1Size = 16;

    uint16 AudioFormat = 1; // PCM
    uint16 NumChannels = 0;
    uint32 SampleRate = 0;
    uint32 ByteRate = 0;
    uint16 BlockAlign = 0;
    uint16 BitsPerSample = 16;

    char Data[4] = { 'd', 'a', 't', 'a' };
    uint32 Subchunk2Size = 0;
};

#pragma pack(pop)

static_assert(sizeof(FWAVHeader) == 44);

FCriticalSection AudioParser::CacheCriticalSection;
TMap<TWeakObjectPtr<USoundWave>, TArray<uint8>> AudioParser::CachedWavDataMap;

USoundWave* AudioParser::FromFlacBytes(const uint8* Bytes, const int64 ByteCount, const FString& ClipName)
{
	return FromAudioBytes(
		Bytes,
		ByteCount,
		TEXT(".flac"),
		ClipName
	);
}

USoundWave* AudioParser::FromAudioBytes(
    const uint8* Bytes,
    const int64 ByteCount,
    const FString& Extension,
    const FString& ClipName)
{
    if (!Bytes || ByteCount <= 0)
    {
        UE_LOG(LogTemp, Error,
            TEXT("AudioParser: Invalid audio data."));
        return nullptr;
    }

    // Decode the source audio into signed 16-bit PCM.
    ma_decoder Decoder{};

    const ma_decoder_config Config =
        ma_decoder_config_init(ma_format_s16, 0, 0);

    ma_result Result = ma_decoder_init_memory(
        Bytes,
        static_cast<size_t>(ByteCount),
        &Config,
        &Decoder
    );

    if (Result != MA_SUCCESS)
    {
        UE_LOG(LogTemp, Error,
            TEXT("AudioParser: miniaudio initialization failed: %d"),
            static_cast<int32>(Result));

        return nullptr;
    }

    ma_uint64 TotalFrameCount = 0;

    Result = ma_decoder_get_length_in_pcm_frames(
        &Decoder,
        &TotalFrameCount
    );

    if (Result != MA_SUCCESS || TotalFrameCount == 0)
    {
        ma_decoder_uninit(&Decoder);

        UE_LOG(LogTemp, Error,
            TEXT("AudioParser: Failed to determine audio length."));

        return nullptr;
    }

    const uint16 NumChannels =
        static_cast<uint16>(Decoder.outputChannels);

    const uint32 SampleRate = Decoder.outputSampleRate;

    if (NumChannels == 0 || SampleRate == 0)
    {
        ma_decoder_uninit(&Decoder);

        UE_LOG(LogTemp, Error,
            TEXT("AudioParser: Invalid audio format."));

        return nullptr;
    }

    // Calculate the PCM buffer size safely.
    const uint64 TotalSamples = TotalFrameCount * NumChannels;
    const uint64 PCMDataSize64 = TotalSamples * sizeof(int16);

    if (PCMDataSize64 == 0 ||
        PCMDataSize64 > MAX_int32 ||
        PCMDataSize64 > MAX_uint32 ||
        TotalFrameCount > MAX_int64)
    {
        ma_decoder_uninit(&Decoder);

        UE_LOG(LogTemp, Error,
            TEXT("AudioParser: Audio data is too large."));

        return nullptr;
    }

    const uint32 PCMDataSize =
        static_cast<uint32>(PCMDataSize64);

    TArray<uint8> PCMData;
    PCMData.SetNumUninitialized(static_cast<int32>(PCMDataSize));

    ma_uint64 FramesRead = 0;

    Result = ma_decoder_read_pcm_frames(
        &Decoder,
        PCMData.GetData(),
        TotalFrameCount,
        &FramesRead
    );

    ma_decoder_uninit(&Decoder);

    if (Result != MA_SUCCESS || FramesRead != TotalFrameCount)
    {
        UE_LOG(LogTemp, Error,
            TEXT("AudioParser: PCM read failed. Result=%d, Frames=%llu/%llu"),
            static_cast<int32>(Result),
            static_cast<unsigned long long>(FramesRead),
            static_cast<unsigned long long>(TotalFrameCount));

        return nullptr;
    }

    // Build a proper WAV file for the existing export/cache logic.
    FWAVHeader Header;

    Header.NumChannels = NumChannels;
    Header.SampleRate = SampleRate;
    Header.BitsPerSample = 16;

    Header.ByteRate =
        SampleRate * NumChannels * sizeof(int16);

    Header.BlockAlign =
        NumChannels * sizeof(int16);

    Header.Subchunk2Size = PCMDataSize;
    Header.ChunkSize = 36 + PCMDataSize;

    TArray<uint8> FullWavFile;
    FullWavFile.SetNumUninitialized(
        sizeof(FWAVHeader) + PCMDataSize
    );

    FMemory::Memcpy(
        FullWavFile.GetData(),
        &Header,
        sizeof(FWAVHeader)
    );

    FMemory::Memcpy(
        FullWavFile.GetData() + sizeof(FWAVHeader),
        PCMData.GetData(),
        PCMDataSize
    );

    // Create a procedural SoundWave that accepts runtime PCM.
    USoundWaveProcedural* SoundWave =
        NewObject<USoundWaveProcedural>(
            GetTransientPackage(),
            *ClipName,
            RF_Transient
        );

    if (!SoundWave)
    {
        return nullptr;
    }

    SoundWave->SetSampleRate(SampleRate);
    SoundWave->NumChannels = NumChannels;
    SoundWave->Duration =
        static_cast<float>(FramesRead) /
        static_cast<float>(SampleRate);

    SoundWave->SoundGroup = SOUNDGROUP_Default;

    // Critical: give Unreal the actual decoded PCM data.
    SoundWave->QueueAudio(
        PCMData.GetData(),
        static_cast<int32>(PCMDataSize)
    );

    // Preserve the WAV data for FromSoundWave() and FFmpeg.
    {
        FScopeLock Lock(&CacheCriticalSection);

        CachedWavDataMap.Add(
            SoundWave,
            MoveTemp(FullWavFile)
        );
    }

    UE_LOG(LogTemp, Log,
        TEXT("AudioParser: Created '%s': %u Hz, %u channels, %.2f seconds"),
        *ClipName,
        SampleRate,
        NumChannels,
        SoundWave->Duration);

    return SoundWave;
}

TArray<uint8> AudioParser::FromSoundWave(USoundWave* Clip, const FString& OutputExtension)
{
    TArray<uint8> EncodedBytes;

    if (!Clip)
    {
        return EncodedBytes;
    }
	
	TArray<uint8> WavData;
	{
    	FScopeLock Lock(&CacheCriticalSection);
    	if (const TArray<uint8>* FoundData = CachedWavDataMap.Find(Clip))
    	{
    		WavData = *FoundData;
    	}
	}

	if (WavData.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("AudioParser: SoundWave was not found in runtime cache or has no cached WAV data."));
		return EncodedBytes;
	}
	
    const FString TempDir = FPaths::ProjectSavedDir() / TEXT("TempAudio/");
    IFileManager::Get().MakeDirectory(*TempDir, true);

    const FString TempInputPath = TempDir / TEXT("temp_in.wav");
    const FString TempOutputPath = TempDir / FString::Printf(TEXT("temp_out%s"), *OutputExtension);
	
    if (!FFileHelper::SaveArrayToFile(WavData, *TempInputPath))
    {
        return EncodedBytes;
    }
	
    const FString FFmpegPath = FPaths::Combine(FPaths::ProjectDir(), TEXT("Binaries/Win64/ffmpeg.exe"));
    const FString Params = FString::Printf(TEXT("-y -i \"%s\" \"%s\""), *TempInputPath, *TempOutputPath);
	
    int32 ReturnCode = -1;
    FProcHandle ProcHandle = FPlatformProcess::CreateProc(
        *FFmpegPath,
        *Params,
        true,   // bLaunchDetached
        true,   // bLaunchHidden
        true,   // bLaunchReallyHidden
        nullptr, 0, nullptr, nullptr
    );

    if (ProcHandle.IsValid())
    {
        FPlatformProcess::WaitForProc(ProcHandle);
        FPlatformProcess::GetProcReturnCode(ProcHandle, &ReturnCode);
        FPlatformProcess::CloseProc(ProcHandle);
    }
	
    if (ReturnCode == 0 && FPaths::FileExists(TempOutputPath))
    {
        FFileHelper::LoadFileToArray(EncodedBytes, *TempOutputPath);
    }
	
    IFileManager::Get().Delete(*TempInputPath);
    IFileManager::Get().Delete(*TempOutputPath);

    return EncodedBytes;
}