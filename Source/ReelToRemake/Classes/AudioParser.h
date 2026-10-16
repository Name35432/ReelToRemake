#pragma once

#include "CoreMinimal.h"
#include "Sound/SoundWave.h"
#include "Containers/Map.h"
#include "Containers/Array.h"

class REELTOREMAKE_API AudioParser
{
public:
    static USoundWave* FromFlacBytes(const uint8* Bytes, int64 ByteCount, const FString& ClipName);
    static USoundWave* FromAudioBytes(const uint8* Bytes, int64 ByteCount, const FString& Extension, const FString& ClipName);
    static TArray<uint8> FromSoundWave(USoundWave* Clip, const FString& OutputExtension);
    
private:
    static FCriticalSection CacheCriticalSection;
    static TMap<TWeakObjectPtr<USoundWave>, TArray<uint8>> CachedWavDataMap;
};