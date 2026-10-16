// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BitChart.generated.h"

/**
 * 
 */
UCLASS()
class REELTOREMAKE_API ABitChart : public AActor
{
	GENERATED_BODY()

public:
	ABitChart();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AirPressure = 80.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DualAirPressure = 60.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<bool> TopDrawer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<bool> BottomDrawer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int> Dmx;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int> Av;
	
protected:
	virtual void BeginPlay() override;


public:
	virtual void Tick(float DeltaTime) override;

	void SetData(bool bBottom, int Index, bool bValue);
};
