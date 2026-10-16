// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProgrammingGroups.h"
#include "InputHandler.generated.h"

USTRUCT(BlueprintType)
struct FInputHandlerData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    bool bAnyButtonHeld = false;

    UPROPERTY(BlueprintReadOnly)
    TArray<bool> TopDrawer;

    UPROPERTY(BlueprintReadOnly)
    TArray<bool> BottomDrawer;

    UPROPERTY(BlueprintReadOnly)
    TArray<int32> Dmx;

    UPROPERTY(BlueprintReadOnly)
    TArray<int32> Av;

    FInputHandlerData()
    {
        TopDrawer.SetNumZeroed(150);
        BottomDrawer.SetNumZeroed(150);
        Dmx.SetNumZeroed(512);
        Av.SetNumZeroed(40);
    }
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class REELTOREMAKE_API UInputHandler : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInputHandler();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation")
    int32 ValveMapping = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation")
    UProgrammingGroups* ProgrammingGroups = nullptr;

    UFUNCTION(BlueprintCallable, Category = "Simulation")
    FInputHandlerData InputCheck();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
    FInputHandlerData FrameInputs;

    bool Inverts[33] = { false };

    float InvertCooldown = 0.0f;

    TArray<FKey> ValveKeyMappings;
    TArray<FKey> EditorKeyMappings;

    void SetupKeyMappings();
};
