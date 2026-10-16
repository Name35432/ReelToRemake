// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/Texture2D.h"
#include "ProgrammingGroups.generated.h"

USTRUCT(BlueprintType)
struct FInputNames
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDrawer = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int32> Bits;
};

USTRUCT(BlueprintType)
struct FMovementRecordings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString GroupName = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* GroupIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FInputNames> InputNames;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class REELTOREMAKE_API UProgrammingGroups : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UProgrammingGroups();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FMovementRecordings> RecordingGroups;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
