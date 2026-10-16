// These are Reel To Remake assets. If you are seeing this, you are in big trouble pal!

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../Animation/LightController.h"
#include "GameFramework/Actor.h"
#include "ShowMain.generated.h"

// Class forwarding for faster compiling
class UMediaPlayer;
class UStaticMeshComponent;
class UMaterialInterface;
class UCharacterMovements;
class UShowManager;
class UInputHandler;
class ABitChart;

UENUM()
enum class EStageType : uint8
{
	Standard UMETA(DisplayName = "Standard"),
	Mini UMETA(DisplayName = "Mini"),
	Retrofit UMETA(DisplayName = "Retrofit"),
	Unofficial UMETA(DisplayName = "Unofficial"),
};

USTRUCT( BlueprintType, meta=(BlueprintSpawnableComponent) )
struct FCharacterSpawn
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString CharacterName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Position;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator Rotation;
};

UENUM()
enum class ETvSetting : uint8
{
	None UMETA(DisplayName = "None"),
	OnWhenCurtain UMETA(DisplayName = "On When Curtain"),
	BitControlledTwoBit UMETA(DisplayName = "Bit Controlled Two Bit"),
	BitControlledOneBit UMETA(DisplayName = "Bit Controlled One Bit"),
	AvControlled UMETA(DisplayName = "Av Controlled"),
};

USTRUCT( BlueprintType, meta=(BlueprintSpawnableComponent) )
struct FShowTv
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stage")
	FString TvName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stage")
	TArray<UStaticMeshComponent*> TvScreens;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stage")
	UMaterialInterface* CurrentMaterial;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stage")
	ETvSetting TvSetting = ETvSetting::None;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Setting")
	int BitOne = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Setting")
	int BitTwo = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Setting")
	int CurtainIndex = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Setting")
	int AvChannel = 0;
};

USTRUCT( BlueprintType, meta=(BlueprintSpawnableComponent) )
struct FStages
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stage")
	FString StageName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stage")
	AActor* StageObject;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attributes")
	FString StageDesc;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attributes")
	FString StageDate;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attributes")
	UTexture2D* StageIcon;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attributes")
	EStageType StageType = EStageType::Standard;
	
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Objects")
	// UCurtainController* Curtains;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Objects")
	AActor* Lights;
	
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Objects")
	//TArray<UTurntableController> TurnTables;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Objects")
	TArray<FCharacterSpawn> StageCharacters;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Objects")
	TArray<FShowTv> Tvs;
	
	TArray<ULightController*> Lighting;
	
	void Startup(AActor* Owner)
	{
		Lighting.Reset();

		if (!IsValid(Owner))
		{
			UE_LOG(LogTemp, Warning, TEXT("FStages::Startup: Owner is invalid."));
			return;
		}

		TArray<AActor*> ActorsToCheck;
		ActorsToCheck.AddUnique(Owner);
		
		if (IsValid(Lights))
		{
			ActorsToCheck.AddUnique(Lights);

			TArray<AActor*> AttachedActors;
			Lights->GetAttachedActors(AttachedActors, true, true);

			for (AActor* AttachedActor : AttachedActors)
			{
				if (IsValid(AttachedActor))
				{
					ActorsToCheck.AddUnique(AttachedActor);
				}
			}
		}
		
		for (const AActor* Actor : ActorsToCheck)
		{
			if (!IsValid(Actor))
			{
				continue;
			}

			TArray<ULightController*> ActorControllers;
			Actor->GetComponents<ULightController>(ActorControllers);

			for (ULightController* Controller : ActorControllers)
			{
				if (!Controller)
				{
					continue;
				}

				Lighting.AddUnique(Controller);
			}
		}
	}
};

UENUM()
enum class ECostumeType
{
	Costume UMETA(DisplayName = "Costume"),
	Retrofit UMETA(DisplayName = "Retrofit"),
	Innards UMETA(DisplayName = "Innards"),
};

USTRUCT( BlueprintType, meta=(BlueprintSpawnableComponent) )
struct FCharacterCostume
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString CostumeName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString CostumeDesc;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString CostumeDate;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CostumeIcon;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECostumeType CostumeType = ECostumeType::Costume;
};

USTRUCT( BlueprintType, meta=(BlueprintSpawnableComponent) )
struct FCharacterSelector
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString CharacterName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> CharacterBlueprint;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CharacterIcon;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CurrentCostume = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FCharacterCostume> AllCostumes;
};

UENUM()
enum class ESignalSettings : uint8
{
	Normal UMETA(DisplayName = "Normal"),
	Crae UMETA(DisplayName = "Crae"),
	Wp5 UMETA(DisplayName = "Wp5"),
	Programblue UMETA(DisplayName = "Programblue"),
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class REELTOREMAKE_API UShowMain : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UShowMain();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stage / Characters")
	TArray<FStages> Stages;
	int CurrentStage = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stage / Characters")
	TArray<FCharacterSelector> SpawnableCharacters;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Stage / Characters")
	TArray<FCharacterSelector> OnStageCharacters;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Assignations")
	TArray<UAudioComponent*> SpeakersRight;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Assignations")
	TArray<UAudioComponent*> SpeakersLeft;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Assignations")
	TArray<UMediaPlayer*> VideoPlayers;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Assignations")
	TArray<UMaterialInterface*> TvMaterials;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Assignations")
	AActor* CharacterHolder;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Assignations")
	ABitChart* BitChart;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Assignations")
	UInputHandler* InputHandler;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Assignations")
	ESignalSettings SignalChange = ESignalSettings::Normal;
	
private:
	bool bMayNotUpdate = false;
	bool FinalTopDrawer[150];
	bool FinalBottomDrawer[150];
	UShowManager* Manager;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	TArray<TObjectPtr<UCharacterMovements>> CachedMovements;
	void RefreshCharacterMovements();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void RecreateCharacters(FString Character);
	void LoadAudio();
	void SyncAudio();
	void Pause();
	void Play();
	void FFSong(int Input);
	void HandleTvs();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Objects")
	bool bRefreshFlows = false;
	
private:
	bool GetBitState(int Bit) const;
	bool GetTwoBitState(int Bit1, int Bit2) const;
	void SetTvMaterial(FShowTv& Tv, int MaterialIndex);
};
