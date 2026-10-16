// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterMovements.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "UObject/UnrealType.h"
#include "BitChart.h"
#include "EngineUtils.h"

// Sets default values for this component's properties
UCharacterMovements::UCharacterMovements()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UCharacterMovements::SetMovementMesh(USkeletalMeshComponent* NewMesh)
{
    if (Mesh == NewMesh && IsValid(AnimInstance))
    {
        return;
    }

    Mesh = NewMesh;

    RebindAnimationVariables();
}

void UCharacterMovements::RebindAnimationVariables()
{
    Bits.Empty();
    AnimInstance = nullptr;

    if (!IsValid(Mesh))
    {
        return;
    }

    AnimInstance = Mesh->GetAnimInstance();

    if (!IsValid(AnimInstance))
    {
        return;
    }

    const UClass* AnimClass = AnimInstance->GetClass();

    for (TFieldIterator<FProperty> It(AnimClass); It; ++It)
    {
        FProperty* Property = *It;
    	
        FDoubleProperty* NumericProperty = CastField<FDoubleProperty>(Property);

        if (!NumericProperty)
        {
            continue;
        }

        const FString VariableName = Property->GetName();

        if (VariableName == TEXT("ANIMATION_TIME"))
        {
            continue;
        }
    	
        if (VariableName.Len() < 2)
        {
            continue;
        }

        const FString BitString = VariableName.LeftChop(1);
        const int32 ParsedBit = FCString::Atoi(*BitString);

        if (ParsedBit <= 0)
        {
            continue;
        }

        FBits Bit;
        Bit.Bit = ParsedBit;
        Bit.bBottomDrawer = VariableName.Contains(TEXT("B"));
        Bit.FloatProperty = NumericProperty;

        Bits.Add(Bit);
    }

    const int32 NumBits = Bits.Num();
	
    CharacterValves.Init(0.f, NumBits);
    MovementDelayTimers.Init(0.f, NumBits);
    HeavyChartIn.Init(0.f, NumBits);
    HeavyChartOut.Init(0.f, NumBits);
    LastBitState.Init(false, NumBits);
}

// Called when the game starts
void UCharacterMovements::BeginPlay()
{
	Super::BeginPlay();
	
	// ...
	if (!IsValid(BitChart))
	{
		for (const TActorIterator<ABitChart> It(GetWorld()); It;)
		{
			BitChart = *It;
			break;
		}
	}
	
	if (!IsValid(Mesh))
	{
		Mesh = GetOwner()->FindComponentByClass<USkeletalMeshComponent>();
	}

	RebindAnimationVariables();
}

// Called every frame
void UCharacterMovements::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UCharacterMovements::UpdateMovements(const float DeltaTime)
{
	Scale = BitChart->AirPressure * PsiScale / 17 * DeltaTime;

	for (int32 i = 0; i < Bits.Num(); i++)
	{
		const auto& [Bit, bBottomDrawer, FloatProperty] = Bits[i];

		bool bOnOff;
		if (bBottomDrawer)
		{
			bOnOff = BitChart->BottomDrawer[Bit - 1];
		}
		else
		{
			bOnOff = BitChart->TopDrawer[Bit - 1];
		}

		if (bUseDp)
		{
			if (DualPressureBit >= 150)
			{
				if (const int Final = DualPressureBit - 150; BitChart->BottomDrawer[Final - 1])
					Scale = BitChart->DualAirPressure * PsiScale / 17 * DeltaTime;
			}
			else
			{
				if (BitChart->BottomDrawer[DualPressureBit - 1])
					Scale = BitChart->DualAirPressure * PsiScale / 17 * DeltaTime;
			}
		}

		if (bMovementDelayEnable)
		{
			if (bOnOff != LastBitState[i])
			{
				LastBitState[i] = bOnOff;

				if (bOnOff)
				{
					MovementDelayTimers[i] = MovementDelayOut[i];
				}
				else
				{
					MovementDelayTimers[i] = MovementDelayIn[i];
				}
			}

			if (MovementDelayTimers[i] > 0.f)
			{
				MovementDelayTimers[i] -= 10 * DeltaTime;
				continue;
			}
		}

		if (bOnOff)
		{
			HeavyChartOut[i] = FMath::Min(HeavyChartOut[i] + SpeedOut[i] * SpeedOut[i] / 2.f, 1.f + (1.f - SpeedOut[i]) * 0.3f);
			HeavyChartIn[i] = 0.f;
			CharacterValves[i] += Scale * SpeedOut[i] * HeavyChartOut[i] * AccelerationOut[i];
		}
		else
		{
			HeavyChartIn[i] = FMath::Min(HeavyChartIn[i] + SpeedIn[i] * SpeedIn[i] / 2.f, 1.f + (1.f - SpeedIn[i]) * 0.3f);
			HeavyChartOut[i] = 0.f;
			CharacterValves[i] -= Scale * SpeedIn[i] * HeavyChartIn[i] * AccelerationIn[i];
		}

		CharacterValves[i] = FMath::Clamp(CharacterValves[i], 0.f, 1.f);
		FloatProperty->SetPropertyValue_InContainer(AnimInstance, CharacterValves[i]);
	}
}