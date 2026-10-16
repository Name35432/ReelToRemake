// Fill out your copyright notice in the Description page of Project Settings.


#include "InputHandler.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "ProgrammingGroups.h"

// Sets default values for this component's properties
UInputHandler::UInputHandler()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UInputHandler::BeginPlay()
{
	Super::BeginPlay();

	// ...
	SetupKeyMappings();
}


// Called every frame
void UInputHandler::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...

}

void UInputHandler::SetupKeyMappings()
{
    ValveKeyMappings = {
        EKeys::Four, EKeys::R, EKeys::F, EKeys::V, EKeys::Five, EKeys::T, EKeys::G, EKeys::B,
        EKeys::Six, EKeys::Y, EKeys::H, EKeys::N, EKeys::Seven, EKeys::U, EKeys::J, EKeys::M,
        EKeys::Eight, EKeys::I, EKeys::K, EKeys::Comma, EKeys::Nine, EKeys::O, EKeys::L, EKeys::Period,
        EKeys::Zero, EKeys::P, EKeys::Semicolon, EKeys::Slash, EKeys::Hyphen, EKeys::LeftBracket,
        EKeys::Apostrophe, EKeys::Equals, EKeys::RightBracket
    };

    EditorKeyMappings = {
        EKeys::One, EKeys::Two, EKeys::Three, EKeys::Four, EKeys::Five,
        EKeys::Six, EKeys::Seven, EKeys::Eight, EKeys::Nine, EKeys::Zero
    };
}

FInputHandlerData UInputHandler::InputCheck()
{
    APlayerController* PC = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr;

    // Reset frame input data
    FrameInputs = FInputHandlerData();

    if (!PC)
    {
        return FrameInputs;
    }

    float DeltaTime = GetWorld()->GetDeltaSeconds();

    // Valve Mapping Logic
    if (ValveMapping > 0 && ProgrammingGroups)
    {
        // Assuming ProgrammingGroups has an equivalent structure in UE
        auto& RecordingGroup = ProgrammingGroups->RecordingGroups[ValveMapping - 1];
        int32 InputCount = RecordingGroup.InputNames.Num();

        for (int32 i = 0; i < InputCount; ++i)
        {
            if (RecordingGroup.InputNames[i].Bits.Num() > 0 && RecordingGroup.InputNames[i].Bits[0] > 0)
            {
                bool bApplyKey = false;

                if (ValveKeyMappings.IsValidIndex(i))
                {
                    bApplyKey = PC->IsInputKeyDown(ValveKeyMappings[i]);
                }

                if (InvertCooldown > 0.0f)
                {
                    InvertCooldown -= DeltaTime;
                }

                bool bCtrlDown = PC->IsInputKeyDown(EKeys::LeftControl);

                if (bCtrlDown && bApplyKey && InvertCooldown <= 0.0f)
                {
                    InvertCooldown = 0.05f * InputCount;
                    Inverts[i] = !Inverts[i];
                }

                if (Inverts[i])
                {
                    bApplyKey = !bApplyKey;
                }

                if (bCtrlDown && InvertCooldown > 0.0f)
                {
                    bApplyKey = false;
                }

                if (bApplyKey)
                {
                    FrameInputs.bAnyButtonHeld = true;

                    for (int32 IndexVal : RecordingGroup.InputNames[i].Bits)
                    {
                        int32 TargetIdx = IndexVal - 1;
                        if (RecordingGroup.InputNames[i].bDrawer)
                        {
                            if (FrameInputs.BottomDrawer.IsValidIndex(TargetIdx))
                                FrameInputs.BottomDrawer[TargetIdx] = true;
                        }
                        else
                        {
                            if (FrameInputs.TopDrawer.IsValidIndex(TargetIdx))
                                FrameInputs.TopDrawer[TargetIdx] = true;
                        }
                    }
                }
            }
        }
    }
    else
    {
        for (int32 i = 0; i < EditorKeyMappings.Num(); ++i)
        {
            bool bApplyKey = PC->IsInputKeyDown(EditorKeyMappings[i]);

            if (InvertCooldown > 0.0f)
            {
                InvertCooldown -= DeltaTime;
            }

            bool bCtrlDown = PC->IsInputKeyDown(EKeys::LeftControl);

            if (bCtrlDown && bApplyKey && InvertCooldown <= 0.0f)
            {
                InvertCooldown = 0.2f;
                Inverts[i] = !Inverts[i];
            }

            if (Inverts[i])
            {
                bApplyKey = !bApplyKey;
            }

            if (bCtrlDown && InvertCooldown > 0.0f)
            {
                bApplyKey = false;
            }
        }

        if (ValveMapping == 0)
        {
            FMemory::Memzero(Inverts, sizeof(Inverts));
        }
    }

    return FrameInputs;
}