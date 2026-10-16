// Fill out your copyright notice in the Description page of Project Settings.


#include "BitChart.h"

ABitChart::ABitChart()
{
	PrimaryActorTick.bCanEverTick = false;

	TopDrawer.Init(false, 150);
	BottomDrawer.Init(false, 150);
	Dmx.Init(0, 512);
	Av.Init(0, 40);
}

void ABitChart::BeginPlay()
{
	Super::BeginPlay();
}

void ABitChart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABitChart::SetData(bool bBottom, int Index, bool bValue)
{
	if (bBottom)
	{
		BottomDrawer[Index] = bValue;
	}
	else
	{
		TopDrawer[Index] = bValue;
	}
}