// These are Reel To Remake assets. If you are seeing this, you are in big trouble pal!


#include "ShowMain.h"
#include "GameFramework/Actor.h"
#include "EngineUtils.h"
#include "ShowManager.h"
#include "../Animation/CharacterMovements.h"
#include "Components/AudioComponent.h"
#include "ReelToRemake/Animation/BitChart.h"
#include "ReelToRemake/Animation/LightController.h"
#include "MediaPlayer.h"
#include "MediaTexture.h"

// Sets default values for this component's properties
UShowMain::UShowMain()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UShowMain::BeginPlay()
{
	Super::BeginPlay();

	// ...
	Manager = GetOwner()->GetComponentByClass<UShowManager>();
	if (!Manager)
	{
		bMayNotUpdate = true;
		UE_LOG(LogTemp, Error, L"[UShowMain] The ShowManager was not found. This will cause issues SO FIX IT");
	}
	
	if (!BitChart)
	{
		bMayNotUpdate = true;
		UE_LOG(LogTemp, Error, L"[UShowMain] The BitChart was not found. This will cause issues SO FIX IT");
	}
	
	for (FStages& Stage : Stages)
	{
		Stage.Startup(GetOwner());
	}
	
	RecreateCharacters("");
}

void UShowMain::RefreshCharacterMovements()
{
	CachedMovements.Reset();

	TArray<AActor*> Children;
	CharacterHolder->GetAttachedActors(Children);

	for (const AActor* Child : Children)
	{
		if (!IsValid(Child))
		{
			continue;
		}

		UCharacterMovements* Cm =
			Child->FindComponentByClass<UCharacterMovements>();

		if (IsValid(Cm))
		{
			CachedMovements.Add(Cm);
		}
	}
}

// Called every frame
void UShowMain::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	if (bMayNotUpdate)
		return;
	
	switch (SignalChange)
	{
	case ESignalSettings::Normal:
		break;
	case ESignalSettings::Crae:
		if (BitChart->TopDrawer[39] || BitChart->BottomDrawer[59])
		{
			BitChart->TopDrawer[39] = BitChart->BottomDrawer[59] = true;
		}
		break;
	case ESignalSettings::Wp5:
		FinalTopDrawer[1] = BitChart->TopDrawer[3];
		FinalTopDrawer[2] = BitChart->TopDrawer[1];
		FinalTopDrawer[3] = BitChart->TopDrawer[2];
		FinalTopDrawer[4] = BitChart->TopDrawer[8];
		FinalTopDrawer[5] = BitChart->TopDrawer[9];
        FinalTopDrawer[6] = BitChart->TopDrawer[7];
        FinalTopDrawer[7] = BitChart->TopDrawer[6];
        FinalTopDrawer[8] = BitChart->TopDrawer[5];
        FinalTopDrawer[9] = BitChart->TopDrawer[10];
        FinalTopDrawer[10] = BitChart->TopDrawer[11];
        FinalTopDrawer[11] = BitChart->TopDrawer[12];
        FinalTopDrawer[12] = BitChart->TopDrawer[13];
        FinalTopDrawer[13] = BitChart->TopDrawer[15];
        FinalTopDrawer[14] = BitChart->TopDrawer[16];
        FinalTopDrawer[15] = BitChart->TopDrawer[14];
        FinalTopDrawer[16] = BitChart->TopDrawer[44];
        FinalTopDrawer[17] = BitChart->TopDrawer[40];
        FinalTopDrawer[18] = BitChart->TopDrawer[41];
        FinalTopDrawer[19] = BitChart->TopDrawer[53];
        FinalTopDrawer[20] = BitChart->TopDrawer[50];
        FinalTopDrawer[21] = BitChart->TopDrawer[58];
        FinalTopDrawer[22] = BitChart->TopDrawer[56];
        FinalTopDrawer[23] = BitChart->TopDrawer[59];
        FinalTopDrawer[24] = BitChart->TopDrawer[61];
        FinalTopDrawer[25] = BitChart->TopDrawer[60];
        FinalTopDrawer[26] = BitChart->TopDrawer[29];
        FinalTopDrawer[27] = BitChart->TopDrawer[25];
        FinalTopDrawer[28] = BitChart->TopDrawer[26];
        FinalTopDrawer[29] = BitChart->TopDrawer[22];
        FinalTopDrawer[30] = BitChart->TopDrawer[23];
        FinalTopDrawer[31] = BitChart->TopDrawer[21];
        FinalTopDrawer[32] = BitChart->TopDrawer[24];
        FinalTopDrawer[33] = BitChart->TopDrawer[20];
        FinalTopDrawer[34] = BitChart->TopDrawer[34];
        FinalTopDrawer[35] = BitChart->TopDrawer[30];
        FinalTopDrawer[36] = BitChart->TopDrawer[63];
        FinalTopDrawer[37] = BitChart->BottomDrawer[15];
        FinalTopDrawer[38] = BitChart->BottomDrawer[2];
        FinalTopDrawer[39] = BitChart->BottomDrawer[0];
        FinalTopDrawer[40] = BitChart->BottomDrawer[1];
        FinalTopDrawer[41] = BitChart->BottomDrawer[7];
        FinalTopDrawer[42] = BitChart->BottomDrawer[5];
        FinalTopDrawer[43] = BitChart->BottomDrawer[6];
        FinalTopDrawer[44] = BitChart->BottomDrawer[4];
        FinalTopDrawer[45] = BitChart->BottomDrawer[3];
        FinalTopDrawer[46] = BitChart->BottomDrawer[12];
        FinalTopDrawer[47] = BitChart->BottomDrawer[14];
        FinalTopDrawer[48] = BitChart->BottomDrawer[9];
        FinalTopDrawer[49] = BitChart->BottomDrawer[34];
        FinalTopDrawer[50] = BitChart->BottomDrawer[30];
        FinalTopDrawer[51] = BitChart->BottomDrawer[31];
        FinalTopDrawer[52] = BitChart->BottomDrawer[25];
        FinalTopDrawer[53] = BitChart->BottomDrawer[26];
        FinalTopDrawer[54] = BitChart->BottomDrawer[29];
        FinalTopDrawer[55] = BitChart->BottomDrawer[27];
        FinalTopDrawer[56] = BitChart->BottomDrawer[28];
        FinalTopDrawer[57] = BitChart->BottomDrawer[22];
        FinalTopDrawer[58] = BitChart->BottomDrawer[17];
        FinalTopDrawer[59] = BitChart->BottomDrawer[23];
        FinalTopDrawer[60] = BitChart->BottomDrawer[18];
        FinalTopDrawer[61] = BitChart->BottomDrawer[24];
        FinalTopDrawer[62] = BitChart->BottomDrawer[19];
        FinalTopDrawer[63] = BitChart->BottomDrawer[37];
        FinalTopDrawer[64] = BitChart->BottomDrawer[36];
        FinalTopDrawer[65] = BitChart->TopDrawer[86];
        FinalTopDrawer[66] = BitChart->TopDrawer[84];
        FinalTopDrawer[67] = BitChart->TopDrawer[85];
        FinalTopDrawer[68] = BitChart->TopDrawer[81];
        FinalTopDrawer[69] = BitChart->TopDrawer[80];
        FinalTopDrawer[70] = BitChart->BottomDrawer[72];
        FinalTopDrawer[71] = BitChart->BottomDrawer[73];
        FinalTopDrawer[72] = BitChart->BottomDrawer[70];
        FinalTopDrawer[73] = BitChart->BottomDrawer[71];
        FinalTopDrawer[102] = BitChart->TopDrawer[65];
        FinalTopDrawer[103] = BitChart->TopDrawer[66];
        FinalTopDrawer[104] = BitChart->TopDrawer[67];
        FinalTopDrawer[105] = BitChart->TopDrawer[68];
        FinalTopDrawer[106] = BitChart->TopDrawer[70];
        FinalTopDrawer[107] = BitChart->TopDrawer[71];
        FinalTopDrawer[99] = BitChart->TopDrawer[57];
        FinalTopDrawer[100] = BitChart->BottomDrawer[33];

        for (int I = 0; I < 150; I++)
        {
	        BitChart->TopDrawer[I] = FinalTopDrawer[I];
        }
		
        break;
	case ESignalSettings::Programblue:
		FinalTopDrawer[87] = BitChart->TopDrawer[86];
		FinalTopDrawer[86] = BitChart->TopDrawer[87];
		
		BitChart->TopDrawer[87] = FinalTopDrawer[87];
		FinalTopDrawer[86] = BitChart->TopDrawer[86];
		break;
	}
	
	for (UCharacterMovements* Cm : CachedMovements)
	{
		if (!IsValid(Cm))
			continue;
		
		Cm->UpdateMovements(DeltaTime);
		Cm->UpdateMovements(DeltaTime);
	}
	
	for (ULightController* Light : Stages[CurrentStage].Lighting)
	{
		Light->UpdateLight(DeltaTime);
	}
	
	//Update curtains and Turntables when I have them coded
	
	HandleTvs();
}

void UShowMain::RecreateCharacters(FString Character = "")
{
	if (Character == "")
	{
		TArray<AActor*> Children;
		CharacterHolder->GetAttachedActors(Children);

		for (AActor* Child : Children)
		{
			if (!IsValid(Child))
			{
				continue;
			}

			Child->Destroy(false, false);
		}
		
		for (int i = 0; i < Stages[CurrentStage].StageCharacters.Num(); i++)
		{
			for (int e = 0; e < SpawnableCharacters.Num(); e++)
			{
				if (Stages[CurrentStage].StageCharacters[i].CharacterName == SpawnableCharacters[e].CharacterName)
				{
					if (SpawnableCharacters[e].CurrentCostume != -1)
					{
						if (SpawnableCharacters[e].CharacterBlueprint && IsValid(CharacterHolder))
						{
							FActorSpawnParameters SpawnParams;
							SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

							if (AActor* Char = GetWorld()->SpawnActor<AActor>(SpawnableCharacters[e].CharacterBlueprint, CharacterHolder->GetActorTransform(), SpawnParams); IsValid(Char))
							{
								Char->Rename(*SpawnableCharacters[e].CharacterName);
								Char->AttachToActor(CharacterHolder, FAttachmentTransformRules::KeepWorldTransform);
								Char->SetActorRelativeLocation(Stages[CurrentStage].StageCharacters[i].Position);
								Char->SetActorRelativeRotation(Stages[CurrentStage].StageCharacters[i].Rotation);
								
								TArray<USceneComponent*> Components;
								Char->GetRootComponent()->GetChildrenComponents(true, Components);
								
								UCharacterMovements* CharacterMovement = Char->FindComponentByClass<UCharacterMovements>();
								
								if (CharacterMovement)
								{
									UE_LOG(LogTemp, Warning, TEXT("No CharacterMovements component found on %s"), *Char->GetName());
								}
								
								USkeletalMeshComponent* SelectedSkeleton = nullptr;
								
								for (USceneComponent* Component : Components)
								{
									if (!IsValid(Component))
									{
										continue;
									}
									
									USkeletalMeshComponent* Skeleton = Cast<USkeletalMeshComponent>(Component);
									if (!Skeleton)
									{
										continue;
									}
									
									Component->SetVisibility(false, true);
									Component->SetHiddenInGame(true);
									
									if (Component->GetName() == SpawnableCharacters[e].AllCostumes[SpawnableCharacters[e].CurrentCostume].CostumeName)
									{
										SelectedSkeleton = Skeleton;
									}
								}
								
								if (IsValid(SelectedSkeleton))
								{
									SelectedSkeleton->SetVisibility(true, true);
									SelectedSkeleton->SetHiddenInGame(false, true);

									if (IsValid(CharacterMovement))
									{
										CharacterMovement->SetMovementMesh(SelectedSkeleton);
									}
								}
								else
								{
									UE_LOG(
										LogTemp,
										Warning,
										TEXT("Costume component '%s' was not found on %s"),
										*SpawnableCharacters[e].AllCostumes[SpawnableCharacters[e].CurrentCostume].CostumeName,
										*Char->GetName()
									);
								}
							}
						}
					}
				}
			}
		}
	}
	else
	{
		TArray<AActor*> Children;
		CharacterHolder->GetAttachedActors(Children);

		for (AActor* Child : Children)
		{
			if (!IsValid(Child))
			{
				continue;
			}

			Child->Destroy(false, false);
		}
		
		for (int e = 0; e < SpawnableCharacters.Num(); e++)
		{
			if (Character == SpawnableCharacters[e].CharacterName)
			{
				TArray<bool> Many;
				Many.Init(false, Stages[CurrentStage].StageCharacters.Num());
				
				for (int i = 0; i < Stages[CurrentStage].StageCharacters.Num(); i++)
				{
					if (Stages[CurrentStage].StageCharacters[i].CharacterName == Character)
					{
						Many[i] = true;
					}
				}
				
				for (int g = 0; g < Many.Num(); g++)
				{
					if (SpawnableCharacters[e].CurrentCostume != -1 && Many[g])
					{
						if (SpawnableCharacters[e].CharacterBlueprint && IsValid(CharacterHolder))
						{
							FActorSpawnParameters SpawnParams;
							SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;


							if (AActor* Char = GetWorld()->SpawnActor<AActor>(SpawnableCharacters[e].CharacterBlueprint, CharacterHolder->GetActorTransform(), SpawnParams); IsValid(Char))
							{
								Char->Rename(*SpawnableCharacters[e].CharacterName);
								Char->AttachToActor(CharacterHolder, FAttachmentTransformRules::KeepWorldTransform);
								Char->SetActorRelativeLocation(Stages[CurrentStage].StageCharacters[g].Position);
								Char->SetActorRelativeRotation(Stages[CurrentStage].StageCharacters[g].Rotation);
								
								TArray<USceneComponent*> Components;
								Char->GetRootComponent()->GetChildrenComponents(true, Components);
								
								UCharacterMovements* CharacterMovement = Char->FindComponentByClass<UCharacterMovements>();
								
								if (CharacterMovement)
								{
									UE_LOG(LogTemp, Warning, TEXT("No CharacterMovements component found on %s"), *Char->GetName());
								}
								
								USkeletalMeshComponent* SelectedSkeleton = nullptr;
								
								for (USceneComponent* Component : Components)
								{
									if (!IsValid(Component))
									{
										continue;
									}
									
									USkeletalMeshComponent* Skeleton = Cast<USkeletalMeshComponent>(Component);
									if (!Skeleton)
									{
										continue;
									}
									
									Component->SetVisibility(false, true);
									Component->SetHiddenInGame(true);
									
									if (Component->GetName() == SpawnableCharacters[e].AllCostumes[SpawnableCharacters[e].CurrentCostume].CostumeName)
									{
										SelectedSkeleton = Skeleton;
									}
								}
								
								if (IsValid(SelectedSkeleton))
								{
									SelectedSkeleton->SetVisibility(true, true);
									SelectedSkeleton->SetHiddenInGame(false, true);

									if (IsValid(CharacterMovement))
									{
										CharacterMovement->SetMovementMesh(SelectedSkeleton);
									}
								}
								else
								{
									UE_LOG(
										LogTemp,
										Warning,
										TEXT("Costume component '%s' was not found on %s"),
										*SpawnableCharacters[e].AllCostumes[SpawnableCharacters[e].CurrentCostume].CostumeName,
										*Char->GetName()
									);
								}
							}
						}
					}
				}
			}
		}
	}
	
	RefreshCharacterMovements();
	
	OnStageCharacters.Empty();
	for (FCharacterSpawn& C : Stages[CurrentStage].StageCharacters)
	{
		for (FCharacterSelector& Compare : SpawnableCharacters)
		{
			if (C.CharacterName == Compare.CharacterName)
			{
				OnStageCharacters.Add(Compare);
			}
		}
	}
	
	bRefreshFlows = true;
}

void UShowMain::LoadAudio()
{
	for (UAudioComponent* SpeakerL : SpeakersLeft)
		SpeakerL->SetSound(Manager->ReferenceClip);
	
	for (UAudioComponent* SpeakerR : SpeakersRight)
		SpeakerR->SetSound(Manager->ReferenceClip);
	
	for (int i = 0; i < Manager->VideoPaths.Num(); i++)
	{
		if (Manager->VideoPaths[i] != "")
		{
			VideoPlayers[i]->OpenFile(Manager->VideoPaths[i]);
			VideoPlayers[i]->Play();
			VideoPlayers[i]->Pause();
		}
	}
	
	SyncAudio();
}

void UShowMain::SyncAudio()
{
	for (UAudioComponent* SpeakerL : SpeakersLeft)
		SpeakerL->Play(Manager->CurrentTime);
	
	for (UAudioComponent* SpeakerR : SpeakersRight)
		SpeakerR->Play(Manager->CurrentTime);
	
	for (int i = 0; i < Manager->VideoPaths.Num(); i++)
	{
		if (Manager->VideoPaths[i] != "")
		{
			VideoPlayers[i]->Seek(FTimespan::FromSeconds(Manager->CurrentTime));
		}
	}
}

void UShowMain::Pause()
{
	SyncAudio();
	
	Manager->ReferenceSpeaker->SetPaused(true);
	
	for (UAudioComponent* SpeakerL : SpeakersLeft)
		SpeakerL->SetPaused(true);
	
	for (UAudioComponent* SpeakerR : SpeakersRight)
		SpeakerR->SetPaused(true);
	
	for (int i = 0; i < Manager->VideoPaths.Num(); i++)
	{
		if (Manager->VideoPaths[i] != "")
		{
			VideoPlayers[i]->Pause();
		}
	}
}

void UShowMain::Play()
{
	Manager->ReferenceSpeaker->SetPaused(false);
	
	for (UAudioComponent* SpeakerL : SpeakersLeft)
		SpeakerL->SetPaused(false);
	
	for (UAudioComponent* SpeakerR : SpeakersRight)
		SpeakerR->SetPaused(false);
	
	for (int i = 0; i < Manager->VideoPaths.Num(); i++)
	{
		if (Manager->VideoPaths[i] != "")
		{
			VideoPlayers[i]->Play();
		}
	}
	
	SyncAudio();
}

void UShowMain::HandleTvs()
{
	bool bAnyVideo = false;
	
	for (FString& Path : Manager->VideoPaths)
	{
		if (!Path.IsEmpty())
		{
			bAnyVideo = true;
		}
	}

	for (auto& Stage = Stages[CurrentStage]; FShowTv& TV : Stage.Tvs)
	{
		switch (TV.TvSetting)
		{
		case ETvSetting::None:
			if (!bAnyVideo)
			{
				SetTvMaterial(TV, 0);
				break;
			}
			
			SetTvMaterial(TV, 1);
			break;
		case ETvSetting::OnWhenCurtain:
			{
				if (!bAnyVideo)
				{
					SetTvMaterial(TV, 0);
					break;
				}
				
				SetTvMaterial(TV, 0);

				constexpr bool bOn = false; // For now until I code the curtains
				
				SetTvMaterial(TV, bOn ? 1 : 0);
			}
			break;
		case ETvSetting::BitControlledTwoBit:
			{
				if (!bAnyVideo)
				{
					SetTvMaterial(TV, 0);
					break;
				}
				
				SetTvMaterial(TV, 0);
				
				const bool bOn = GetTwoBitState(TV.BitOne, TV.BitTwo);
				
				SetTvMaterial(TV, bOn ? 1 : 0);
			}
			break;
		case ETvSetting::BitControlledOneBit:
			{
				if (!bAnyVideo)
				{
					SetTvMaterial(TV, 0);
					break;
				}
				
				SetTvMaterial(TV, 0);
				
				const bool bOn = GetBitState(TV.BitOne);
				
				SetTvMaterial(TV, bOn ? 1 : 0);
			}
			break;
		case ETvSetting::AvControlled:
			if (!bAnyVideo)
			{
				SetTvMaterial(TV, 0);
				break;
			}
				
			SetTvMaterial(TV, 0);
			
			int Av = BitChart->Av[TV.AvChannel - 1];
			Av = FMath::Clamp(Av, 0, TvMaterials.Num() - 1);
			
			SetTvMaterial(TV, Av);
			break;
		}
	}
}

bool UShowMain::GetBitState(const int Bit) const
{
	bool bBottomDrawer;
	int FinalBit;
	
	if (Bit > 150)
	{
		FinalBit = Bit - 150;
		bBottomDrawer = true;
	}
	else
	{
		FinalBit = Bit;
		bBottomDrawer = false;
	}
	
	return bBottomDrawer 
		? BitChart->BottomDrawer[FinalBit - 1]
		: BitChart->TopDrawer[FinalBit - 1];
}

bool UShowMain::GetTwoBitState(const int Bit1, const int Bit2) const
{
	bool bOn = GetBitState(Bit1);
	
	if (bOn && GetBitState(Bit2))
	{
		bOn = false;
	}
	
	return bOn;
}

void UShowMain::SetTvMaterial(FShowTv& Tv, const int MaterialIndex)
{
	auto* Mat = TvMaterials[MaterialIndex];
	
	if (Tv.CurrentMaterial == Mat)
		return;
	
	Tv.CurrentMaterial = Mat;
	
	for (const auto& Renderer : Tv.TvScreens)
	{
		Renderer->SetMaterial(0, Mat);
	}
}