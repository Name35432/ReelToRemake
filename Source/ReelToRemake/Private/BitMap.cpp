// Fill out your copyright notice in the Description page of Project Settings.


#include "BitMap.h"

BitMap::BitMap()
{
	TopDrawer.resize(19);
	BottomDrawer.resize(19);
	DmxData.resize(512);
	AvData.resize(40);
	for (int i = 0; i < 19; i++)
	{
		TopDrawer[i] = 0x00;
		BottomDrawer[i] = 0x00;
	}
	for (int i = 0; i < 512; i++)
	{
		DmxData[i] = 0x00;
	}
	for (int i = 0; i < 40; i++)
	{
		AvData[i] = 0x00;
	}
}

void BitMap::SetData(bool bBottom, int Bit, bool bValue)
{
	if (Bit < 0 || Bit >= 150)
		return;

	int ByteIndex = Bit / 8;
	int BitIndex = Bit % 8;

	auto& Drawer = bBottom ? BottomDrawer : TopDrawer;
	if (bValue)
		Drawer[ByteIndex] |= static_cast<uint8_t>(1 << BitIndex);
	else
		Drawer[ByteIndex] &= static_cast<uint8_t>(~(1 << BitIndex));
}

void BitMap::SetData(int ChannelNumber, int ChannelValue, bool bAv)
{
	if (bAv)
	{
		if (ChannelNumber >= 0 && ChannelNumber < 40)
			AvData[ChannelNumber] = static_cast<uint8_t>(ChannelValue);
	}
	else
	{
		if (ChannelNumber >= 0 && ChannelNumber < 512)
			DmxData[ChannelNumber] = static_cast<uint8_t>(ChannelValue);
	}
}

bool BitMap::GetData(bool bBottom, int Bit)
{
	if (Bit < 0 || Bit >= 150)
		return false;

	int ByteIndex = Bit / 8;
	int BitIndex = Bit % 8;

	const auto& Drawer = bBottom ? BottomDrawer : TopDrawer;
	return (Drawer[ByteIndex] & (1 << BitIndex)) != 0;
}

int BitMap::GetData(int ChannelNumber, bool bAv)
{
	if (bAv)
	{
		if (ChannelNumber >= 0 && ChannelNumber < 40)
			return AvData[ChannelNumber];
	}
	else
	{
		if (ChannelNumber >= 0 && ChannelNumber < 512)
			return DmxData[ChannelNumber];
	}
	return 0;
}

void BitMap::SetContinousData(int Index, bool bValue)
{
	if (Index < 0 || Index >= 300)
		return;

	if (Index < 150)
		SetData(false, Index, bValue);
	else
		SetData(true, Index - 150, bValue);
}

bool BitMap::GetContinousData(int Index)
{
	if (Index < 0 || Index >= 300)
		return false;

	if (Index < 150)
		return GetData(false, Index);
	else
		return GetData(true, Index - 150);
}