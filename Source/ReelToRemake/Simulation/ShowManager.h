// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../Animation/BitChart.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundWave.h"
#include <BitMap.h>
#include "InputHandler.h"
#include "ShowManager.generated.h"

class UShowMain;

UENUM(BlueprintType)
enum class EShowLoop : uint8
{
    NoLoop UMETA(DisplayName = "No Loop"),
    LoopSegment UMETA(DisplayName = "Loop Segment"),
    LoopShowtape UMETA(DisplayName = "Loop Showtape")
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class REELTOREMAKE_API UShowManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UShowManager();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inspector Objects")
	ABitChart* BitChart;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inspector Objects")
	UInputHandler* InputHandler;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inspector Objects")
	UShowMain* ShowMain;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Show System")
	UAudioComponent* ReferenceSpeaker;
	
	TArray<BitMap> ShowData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Show System")
	USoundWave* ReferenceClip;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Show System")
	EShowLoop ShowLoop;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Show System")
	TArray<FString> ShowPath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Show System")
	int CurrentShow = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Show System")
	TArray<FString> VideoPaths;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Show System")
	FString OldShowExtension = "rshw";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Show System")
	float DataFps = 60;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation States/Extra")
	bool bRecordData = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation States/Extra")
	bool bPlaybackData = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation States/Extra")
	float SyncTimer = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation States/Extra")
	int CurrentFrame = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation States/Extra")
	int PreviousFrame = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation States/Extra")
	bool bPreviousAnyButtonHeld = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation States/Extra")
	bool bLoadShow = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation States/Extra")
	float CurrentTime = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation States/Extra")
	int DeleteMovementsWindowsPage = 0;

public:
	UFUNCTION()
	void OnPlaybackPercent(const USoundWave* PlayingSoundWave, float PlaybackPercent);

	UFUNCTION(BlueprintCallable, Category = "Show Manager")
	void LoadShow();
	
	UFUNCTION(BlueprintCallable, Category = "Show Manager")
	void LoadFolder();
	
	UFUNCTION(BlueprintCallable, Category = "Show Manager")
	void Play(bool bForce, bool bOnOff);
	
	UFUNCTION(BlueprintCallable, Category = "Show Manager")
	void SaveAs();
	
	UFUNCTION(BlueprintCallable, Category = "Show Manager")
	void SwapLoop();
	
	UFUNCTION(BlueprintCallable, Category = "Show Manager")
	void SkipSegment(int Amount);
	
	UFUNCTION(BlueprintCallable, Category = "Show Manager")
	void Unload();
	
	UFUNCTION(BlueprintCallable, Category = "Show Manager")
	void DeleteMove(int BitDelete);
	
	UFUNCTION(BlueprintCallable, Category = "Show Manager")
	void DeleteBitNoSaving(int BitDelete, bool bFill);
	
	UFUNCTION(BlueprintCallable, Category = "Show Manager")
	void PadMove(int BitPad, int Padding);
	
	UFUNCTION(BlueprintCallable, Category = "Show Manager")
	void PadAllBits(int Padding);
	
	UFUNCTION(BlueprintCallable, Category = "Show Manager")
	void SelectAudio();
	
	UFUNCTION(BlueprintCallable, Category = "Show Manager")
	void CreateShowData();
	
	UFUNCTION(BlueprintCallable, Category = "Show Manager")
	void SaveRecording();
	
	UFUNCTION(BlueprintCallable, Category = "Show Manager")
	bool SaveRecordingAs();
	
	UFUNCTION(BlueprintCallable, Category = "Show Manager")
	void GetVideos(FString Find);
	
	UFUNCTION(BlueprintCallable, Category = "Show Manager")
	void ReplaceShowtapeAudio();
	
	UFUNCTION(BlueprintCallable, Category = "Show Manager")
	void LoadShowtape(FString path);
	
	void OpenFilePanel(const FString& DialogTitle, const FString& DefaultPath, const FString& DefaultFile, const FString& FileTypes, uint32 Flags, TArray<FString>& OutFilenames);

private:
	FInputHandlerData Ihd;
};
