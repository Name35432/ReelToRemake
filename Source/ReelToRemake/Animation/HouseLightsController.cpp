// These are Reel To Remake assets. If you are seeing this, you are in big trouble pal!


#include "HouseLightsController.h"
#include "Components/LightComponent.h"
#include "EngineUtils.h"

// Sets default values for this component's properties
UHouseLightsController::UHouseLightsController()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UHouseLightsController::BeginPlay()
{
	Super::BeginPlay();

	// ...
	AActor* Owner = GetOwner();

	if (!Owner)
	{
		return;
	}

	Lights.Empty();
	LightsOriginalBrightness.Empty();

	TArray<AActor*> ActorsToCheck;
	
	ActorsToCheck.Add(Owner);
	
	TArray<AActor*> AttachedActors;
	Owner->GetAttachedActors(AttachedActors, true, true);

	ActorsToCheck.Append(AttachedActors);

	for (AActor* Actor : ActorsToCheck)
	{
		if (!Actor)
		{
			continue;
		}

		TArray<ULightComponent*> ActorLights;
		Actor->GetComponents<ULightComponent>(ActorLights);

		for (ULightComponent* Light : ActorLights)
		{
			if (!Light)
			{
				continue;
			}

			Lights.AddUnique(Light);
			LightsOriginalBrightness.Add(Light->Intensity);
		}
	}
	
	LightsOriginalBrightness.Init(0.f, Lights.Num());
	for (int I = 0; I < Lights.Num(); I++)
	{
		LightsOriginalBrightness[I] = Lights[I]->Intensity;
	}
	
	for (const TActorIterator<ABitChart> It(GetWorld()); It;)
	{
		MagnaCarta = *It;
		break;
	}
}

float UHouseLightsController::GetFadeDelta(const float DeltaTime) const
{
	float Result = DeltaTime / FadeSpeed;
	if (bLegacyFade)
		Result = (DeltaTime * 60) * FadeSpeed;
	
	return Result;
}


// Called every frame
void UHouseLightsController::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	if (!bNoBit)
	{
		if (!MagnaCarta)
			return;
		
		if (Bit <= 0)
			return;
		
		if (Bit > 150)
			return;

		const bool bState = bBottomDrawer ? MagnaCarta->BottomDrawer[Bit - 1] : MagnaCarta->TopDrawer[Bit - 1];
		
		if (bNonStandard)
		{
			bLastState = !bState;
		}
		else
		{
			if (bState)
			{
				if (!bHolding)
				{
					bHolding = true;
					bLastState = !bLastState;
				}
			}
			else
			{
				if (bHolding)
				{
					bHolding = false;
				}
			}
		}
		
		float Delta = GetFadeDelta(DeltaTime);
		
		if (bLastState)
		{
			NextTime += Delta;
		}
		else
		{
			NextTime -= Delta;
		}
		
		NextTime = FMath::Clamp(NextTime, 0.f, 1.f);
		
		for (int I = 0; I < Lights.Num(); I++)
		{
			Lights[I]->SetIntensity(LightsOriginalBrightness[I] * NextTime);
		}
	}
}

void UHouseLightsController::OnShowLoaded(const bool bOverride)
{
	bLastState = bOverride;
}

void UHouseLightsController::OnShowUnloaded(const bool bOverride)
{
	bLastState = bOverride;
}

