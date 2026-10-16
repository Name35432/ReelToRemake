// Fill out your copyright notice in the Description page of Project Settings.


#include "LightController.h"
#include "EngineUtils.h"

static const FName EmissiveColorParameter(TEXT("EmissiveColor"));

// Sets default values for this component's properties
ULightController::ULightController()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void ULightController::BeginPlay()
{
	Super::BeginPlay();

	// ...
	Light = GetOwner()->FindComponentByClass<ULightComponent>();
	for (TActorIterator<ABitChart> It(GetWorld()); It; ++It)
	{
		BitChart = *It;
		break;
	}

	if (bMaterialLight)
	{
		Renderer = MaterialLightObject->FindComponentByClass<UMeshComponent>();
		if (Renderer)
		{
			UMaterialInterface* mat;
			for (int32 i = 0; i < Renderer->GetNumMaterials(); i++)
			{
				mat = Renderer->GetMaterial(i);

				if (mat && mat->GetName() == MaterialName)
				{
					Material = Renderer->CreateAndSetMaterialInstanceDynamic(i);
					Material->SetVectorParameterValue(EmissiveColorParameter, EmissionColor);
					break;
				}
			}
		}
	}

	if (Light)
	{
		if (Brightness <= 0)
		{
			Brightness = Light->Intensity;
		}
	}
}

float ULightController::FadeDeltaTime(float Dt)
{
	if (bLegacyFade)
	{
		return (Dt * 60) * FadeSpeed;
	}
	else
	{
		return Dt / FadeSpeed;
	}
}


// Called every frame
void ULightController::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void ULightController::UpdateLight(float DeltaTime)
{
	Delta = FadeDeltaTime(DeltaTime);

	if (LightBit > 0 && LightBit <= 150 && BitChart)
	{
		if (bBottomDrawer)
		{
			bOnOff = BitChart->BottomDrawer[LightBit - 1];
		}
		else
		{
			bOnOff = BitChart->TopDrawer[LightBit - 1];
		}

		if (bInvertBit)
			bOnOff = !bOnOff;

		if (bFlash)
		{
			if (bOnOff)
			{
				if (!bFlashCheck)
				{
					bFlashCheck = true;
					FadeDelta = 1;
				}
				else
				{
					FadeDelta -= Delta;
				}
			}
			else
			{
				bFlashCheck = false;
				FadeDelta -= Delta;
			}
		}
		else if (bStrobe)
		{
			if (bOnOff)
			{
				if (FadeDelta != 0)
				{
					FadeDelta -= Delta * 2;
				}
				else
				{
					FadeDelta = 1;
				}
			}
			else
			{
				FadeDelta -= Delta * 2;
			}
		}
		else
		{
			if (bOnOff)
			{
				FadeDelta += Delta;
			}
			else
			{
				FadeDelta -= Delta;
			}
		}

		FadeDelta = FMath::Clamp(FadeDelta, 0.f, 1.f);

		if (bMaterialLight)
		{
			if (Material)
			{
				Material->SetVectorParameterValue(EmissiveColorParameter, EmissionColor * Brightness * FadeDelta);
			}
		}
		else
		{
			if (!Light)
				return;

			Light->SetIntensity(Brightness * FadeDelta);
		}
	}
}