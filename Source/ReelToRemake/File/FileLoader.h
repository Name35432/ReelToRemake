// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <BitMap.h>
#include "Serialization/BufferArchive.h"
#include "Serialization/MemoryReader.h"

struct FNativeRshwData
{
    uint8* AudioData;
    int32 AudioDataLen;

    int32* SignalData;
    int32 SignalDataLen;

    const char* DmxJson;
    const char* AvJson;
};

/**
 * 
 */
class REELTOREMAKE_API FileLoader
{
public:
	static void LoadShow(const FString& FilePath, TArray<BitMap>& ShowData, TArray<uint8>& AudioData);
    static void SaveShow(const FString& FilePath, TArray<BitMap>& ShowData, TArray<uint8>& AudioData);
private:
    static void LoadRshw(const FString& FilePath, TArray<BitMap>& ShowData, TArray<uint8>& AudioData);
    static void LoadRtr(const FString& FilePath, TArray<BitMap>& ShowData, TArray<uint8>& AudioData);
    static bool ParseJaggedJsonArray(const FString& JsonString, TArray<TArray<int32>>& OutArray);
    static int32 Read7BitEncodedInt(FMemoryReader& Ar);
    static void Write7BitEncodedInt(FBufferArchive& Ar, int32 Value);
};
