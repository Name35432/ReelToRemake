// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/LightComponent.h"
#include "Components/MeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "BitChart.h"
#include "LightController.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class REELTOREMAKE_API ULightController : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ULightController();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void UpdateLight(float DeltaTime);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int LightBit = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bBottomDrawer = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Brightness = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.001"))
	float FadeSpeed = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bLegacyFade = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bMaterialLight = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bFlash = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bStrobe = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bInvertBit = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* MaterialLightObject;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor EmissionColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString MaterialName = "";

private:
	UMaterialInstanceDynamic* Material;
	UMeshComponent* Renderer;

	float FadeDelta = 0.f;
	bool bFlashCheck = false;
	ULightComponent* Light;
	ABitChart* BitChart;
	float Delta = 0.f;
	bool bOnOff = false;

protected:
	float FadeDeltaTime(float Dt);
};
