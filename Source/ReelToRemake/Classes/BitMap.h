// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <cstdint>
#include <vector>

/**
 * 
 */
class REELTOREMAKE_API BitMap
{
public:
	std::vector<uint8_t> TopDrawer;
	std::vector<uint8_t> BottomDrawer;
	std::vector<uint8_t> DmxData;
	std::vector<uint8_t> AvData;

public:
	BitMap();

	void SetData(bool bBottom, int Bit, bool bValue);
	void SetData(int ChannelNumber, int ChannelValue, bool bAv);
	bool GetData(bool bBottom, int Bit);
	int GetData(int ChannelNumber, bool bAv);
	void SetContinousData(int Index, bool bValue);
	bool GetContinousData(int Index);
};
