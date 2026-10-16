// Fill out your copyright notice in the Description page of Project Settings.


#include "ShowManager.h"
#include <BitMap.h>
#include "InputHandler.h"
#include "Framework/Application/SlateApplication.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "../File/FileLoader.h"
#include <AudioParser.h>
#include "ProgrammingGroups.h"
#include <FileBrowser.h>

#include "ShowMain.h"

// Sets default values for this component's properties
UShowManager::UShowManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...

}


// Called when the game starts
void UShowManager::BeginPlay()
{
	Super::BeginPlay();

	ReferenceSpeaker = GetOwner()->FindComponentByClass<UAudioComponent>();
	InputHandler = BitChart->FindComponentByClass<UInputHandler>();
	if (InputHandler)
	{
		InputHandler->ProgrammingGroups = GetOwner()->FindComponentByClass<UProgrammingGroups>();
	}
	
	ShowMain = GetOwner()->FindComponentByClass<UShowMain>();

	// ...
	if (ReferenceSpeaker)
	{
		ReferenceSpeaker->OnAudioPlaybackPercent.AddDynamic(this, &UShowManager::OnPlaybackPercent);
	}
}


// Called every frame
void UShowManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...

	if (bLoadShow)
	{
		bLoadShow = false;
		LoadShow();
	}

	if (BitChart && InputHandler)
	{
		Ihd = InputHandler->InputCheck();

		for (int i = 0; i < 150; i++)
		{
			BitChart->TopDrawer[i] = Ihd.TopDrawer[i];
			BitChart->BottomDrawer[i] = Ihd.BottomDrawer[i];
		}
		for (int i = 0; i < 512; i++)
		{
			BitChart->Dmx[i] = Ihd.Dmx[i];
		}
		for (int i = 0; i < 40; i++)
		{
			BitChart->Av[i] = Ihd.Av[i];
		}

		if (ReferenceSpeaker->Sound)
		{
			CurrentFrame = static_cast<int>(CurrentTime * DataFps);

			if (CurrentFrame >= ShowData.Num() && ShowData.Num() > 0)
			{
				if (bRecordData)
				{
					while (CurrentFrame >= ShowData.Num())
					{
						ShowData.Add(BitMap());
					}
				}
				else
				{
					CurrentFrame = ShowData.Num();
				}
			}

			if (bRecordData)
			{
				if (Ihd.bAnyButtonHeld)
				{
					for (int i = 0; i < 150; i++)
					{
						if (Ihd.TopDrawer[i])
							ShowData[CurrentFrame].SetData(false, i, true);

						if (Ihd.BottomDrawer[i])
							ShowData[CurrentFrame].SetData(true, i, true);
					}

					for (int i = 0; i < 512; i++)
					{
						if (Ihd.Dmx[i] > 0)
						{
							ShowData[CurrentFrame].SetData(i, Ihd.Dmx[i], false);
						}
					}

					for (int i = 0; i < 40; i++)
					{
						if (Ihd.Av[i] > 0)
						{
							ShowData[CurrentFrame].SetData(i, Ihd.Av[i], true);
						}
					}

					if (bPreviousAnyButtonHeld)
					{
						if (PreviousFrame <= ShowData.Num())
						{
							for (int e = 0; e < CurrentFrame - PreviousFrame; e++)
							{
								for (int i = 0; i < 150; i++)
								{
									if (Ihd.TopDrawer[i])
										ShowData[PreviousFrame + e].SetData(false, i, true);

									if (Ihd.BottomDrawer[i])
										ShowData[PreviousFrame + e].SetData(true, i, true);
								}

								for (int i = 0; i < 512; i++)
								{
									if (Ihd.Dmx[i] > 0)
									{
										ShowData[PreviousFrame + e].SetData(i, Ihd.Dmx[i], false);
									}
								}

								for (int i = 0; i < 40; i++)
								{
									if (Ihd.Av[i] > 0)
									{
										ShowData[PreviousFrame + e].SetData(i, Ihd.Av[i], true);
									}
								}
							}
						}
						else
						{
							for (int e = 0; e < CurrentFrame - PreviousFrame; e++)
							{
								for (int i = 0; i < 150; i++)
								{
									if (Ihd.TopDrawer[i])
										ShowData[PreviousFrame - e].SetData(false, i, true);

									if (Ihd.BottomDrawer[i])
										ShowData[PreviousFrame - e].SetData(true, i, true);
								}

								for (int i = 0; i < 512; i++)
								{
									if (Ihd.Dmx[i] > 0)
									{
										ShowData[PreviousFrame - e].SetData(i, Ihd.Dmx[i], false);
									}
								}

								for (int i = 0; i < 40; i++)
								{
									if (Ihd.Av[i] > 0)
									{
										ShowData[PreviousFrame - e].SetData(i, Ihd.Av[i], true);
									}
								}
							}
						}
					}
				}
			}

			if (bPlaybackData && CurrentFrame < ShowData.Num())
			{
				for (int i = 0; i < 150; i++)
				{
					BitChart->SetData(false, i, ShowData[CurrentFrame].GetData(false, i));
					BitChart->SetData(true, i, ShowData[CurrentFrame].GetData(true, i));
				}
			}

			PreviousFrame = CurrentFrame;
			bPreviousAnyButtonHeld = Ihd.bAnyButtonHeld;
		}
	}
}

void UShowManager::OnPlaybackPercent(const USoundWave* PlayingSoundWave, float PlaybackPercent)
{
	if (PlayingSoundWave)
	{
		CurrentTime = PlayingSoundWave->GetDuration() * PlaybackPercent;
	}
}

void UShowManager::LoadShow()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	bool bPreviousShowMouseCursor = false;

	if (PC)
	{
		bPreviousShowMouseCursor = PC->bShowMouseCursor;
		PC->bShowMouseCursor = true;
		PC->SetInputMode(FInputModeUIOnly());
	}

	if (ReferenceSpeaker)
	{
		ReferenceSpeaker->Play(0.0f);
		ReferenceSpeaker->Stop();
	}
	
	FString FileTypes;
	if (OldShowExtension.IsEmpty())
	{
		FileTypes = L"*.rtr;*.cshw;*.nshw;*.rshw;*.sshw";
	}
	else
	{
		FileTypes = FString::Printf(TEXT("*.rtr;*.%s"), *OldShowExtension);
	}
	
	TArray<FExtensionFilter> Extentions;
	
	FExtensionFilter ShowFiles;
	ShowFiles.Description = L"Show Files";
	ShowFiles.Extension = FileTypes;
	Extentions.Add(ShowFiles);
	
	FExtensionFilter AllFiles;
	AllFiles.Description = L"All Files (FOR MISC SHOWS)";
	AllFiles.Extension = L"*.*";
	Extentions.Add(AllFiles);

	if (TArray<FString> OpenFilenames = UFileDialogLibrary::OpenFilePanelMultiFilter(L"Select your show you want to play", L"", Extentions, false); !OpenFilenames.IsEmpty())
	{
		if (!OpenFilenames[0].IsEmpty())
		{
			if (ShowPath.Num() == 0)
			{
				ShowPath.Add(OpenFilenames[0]);
			}
			else
			{
				ShowPath[0] = OpenFilenames[0];
			}

			CurrentShow = 0;

			LoadShowtape(ShowPath[CurrentShow]);
		}
	}

	if (PC)
	{
		PC->bShowMouseCursor = bPreviousShowMouseCursor;
		PC->SetInputMode(FInputModeGameOnly());
	}
}

void UShowManager::LoadFolder()
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		PC->bShowMouseCursor = true;
		PC->SetInputMode(FInputModeUIOnly());
	}

	if (ReferenceSpeaker)
	{
		ReferenceSpeaker->Play(0.0f);
		ReferenceSpeaker->Stop();
	}

	if (TArray<FString> FolderPaths = UFileDialogLibrary::OpenFolderPanel(L"Select a folder of show you want to play", L"", false); !FolderPaths.IsEmpty())
	{
		if (!FolderPaths[0].IsEmpty())
		{			
			IFileManager& FileManager = IFileManager::Get();
			
			FileManager.FindFiles(ShowPath, *(FPaths::Combine(FolderPaths[0], TEXT("*.rtr"))), true, false );
			
			TArray<FString> OldShowPaths;
			FileManager.FindFiles(OldShowPaths, *(FPaths::Combine(FolderPaths[0], TEXT("*.") + OldShowExtension)), true, false );
			ShowPath.Append(OldShowPaths);
			
			CurrentShow = 0;
			
			LoadShowtape(ShowPath[CurrentShow]);
		}
	}
}

void UShowManager::ReplaceShowtapeAudio()
{
}

void UShowManager::LoadShowtape(FString path)
{
	bPlaybackData = true;
	
	CurrentTime = 0.0f;
	CurrentFrame = 0;
	PreviousFrame = 0;

	TArray<uint8> AudioData;
	TArray<BitMap> Temp;
	FileLoader::LoadShow(path, Temp, AudioData);
	ReferenceClip = AudioParser::FromFlacBytes(AudioData.GetData(), AudioData.Num(), "Hotdog");
	ReferenceSpeaker->SetSound(ReferenceClip);
	ShowData = Temp;
	ReferenceSpeaker->Play();
}

void UShowManager::Play(const bool bForce, const bool bOnOff)
{
	if (bForce)
	{
		bPlaybackData = bOnOff;
	}
	else
	{
		bPlaybackData = !bPlaybackData;
	}
}

void UShowManager::SaveAs()
{
	SaveRecordingAs();
}

void UShowManager::SwapLoop()
{
	switch (ShowLoop)
	{
	case EShowLoop::LoopShowtape:
		ShowLoop = EShowLoop::LoopSegment;
		break;
	case EShowLoop::LoopSegment:
		ShowLoop = EShowLoop::NoLoop;
		break;
	case EShowLoop::NoLoop:
		ShowLoop = EShowLoop::LoopShowtape;
		break;
	}
}

void UShowManager::SkipSegment(const int Amount)
{
	bPlaybackData = false;
	ReferenceSpeaker->Play(0.f);
	ReferenceSpeaker->Stop();
	
	if (ShowLoop == EShowLoop::NoLoop || ShowLoop == EShowLoop::LoopShowtape)
		CurrentShow += Amount;
	
	if (CurrentShow < 0)
		CurrentShow = 0;
	else if (CurrentShow >= ShowPath.Num())
	{
		if (ShowLoop == EShowLoop::LoopShowtape)
			CurrentShow = 0;
		else
			CurrentShow = ShowPath.Num() - 1;
	}
	
	LoadShowtape(ShowPath[CurrentShow]);
}

void UShowManager::Unload()
{
	for (FString& VidPath : VideoPaths)
		VidPath.Empty();
	
	bPlaybackData = false;
	bRecordData = false;
	ReferenceSpeaker->Play(0.f);
	ReferenceSpeaker->Stop();
	CurrentShow = -1;
	ShowPath.Init(TEXT(""), 1);
	ShowData.Empty();
	InputHandler->ValveMapping = 0;
}

void UShowManager::DeleteMove(const int BitDelete)
{
	const int NewInput = BitDelete + (24 * DeleteMovementsWindowsPage);
	UE_LOG(LogTemp, Log, TEXT("Deleting Move Of: %d"), NewInput);

	const FString Ext = FString::Printf(TEXT("*.rtr;*.%s"), *OldShowExtension);
	
	ShowPath.Init(TEXT(""), 1);
	ShowPath = UFileDialogLibrary::OpenFilePanel(L"Select the show you want to delete bits from", L"", Ext, false);
	if (!ShowPath.IsEmpty())
	{
		if (!ShowPath[0].IsEmpty())
		{
			const FString IsRtr = FPaths::GetExtension(ShowPath[0], false);
			
			bool bOldFormat = true;
			if (IsRtr.Contains("rtr", ESearchCase::IgnoreCase))
				bOldFormat = false;
			
			TArray<BitMap> DataToModify;
			TArray<uint8> AudioData;
			FileLoader::LoadShow(ShowPath[0], DataToModify, AudioData);
			
			if (bOldFormat)
			{
				ReferenceClip = AudioParser::FromAudioBytes(AudioData.GetData(), AudioData.Num(), ".wav", "RSHW Out");
			}
			else
			{
				ReferenceClip = AudioParser::FromFlacBytes(AudioData.GetData(), AudioData.Num(), "RTR Out");
			}
			
			for (BitMap& Frame : DataToModify)
				Frame.SetContinousData(NewInput - 1, false);
			
			FString SavePath = "";
			if (bOldFormat)
			{
				SavePath = ShowPath[0].Mid(0, IsRtr.Len() - 1) + TEXT(".rtr");
			}
			else
			{
				SavePath = ShowPath[0];
			}
			
			AudioData = AudioParser::FromSoundWave(ReferenceClip, ".flac");
			FileLoader::SaveShow(SavePath, DataToModify, AudioData);
		}
	}
}

void UShowManager::DeleteBitNoSaving(const int BitDelete, const bool bFill)
{
	UE_LOG(LogTemp, Log, TEXT("Deleting Bit (No Save): %d"), BitDelete);
	
	for (BitMap Frame : ShowData)
		Frame.SetContinousData(BitDelete - 1, bFill);
}

void UShowManager::PadMove(int BitPad, int Padding)
{
	BitPad -= 1;
	if (Padding > 0)
	{
		const int OldLength = ShowData.Num();
                
		for (int i = 0; i < Padding; i++)
		{
			ShowData.Add(BitMap());
		}
		for (int e = 0; e < OldLength; e++)
		{
			ShowData[ShowData.Num() - 1 - e].SetContinousData(BitPad, ShowData[OldLength - 1 - e].GetContinousData(BitPad));
		}
	}
	else
	{
		Padding = FMath::Abs(Padding);
		for (int i = 0; i < ShowData.Num() - Padding; i++)
		{
			ShowData[i].SetContinousData(BitPad, ShowData[i + Padding].GetContinousData(BitPad));
		}
	}
}

void UShowManager::PadAllBits(const int Padding)
{
	if (!ShowData.IsEmpty())
	{
		int Address = static_cast<int>(CurrentTime * DataFps);
                
		if (Padding > 0)
		{
			for (int i = 0; i < Padding; i++)
			{
				ShowData.Add(BitMap());
			}

			int Track = ShowData.Num();
			while (Track > Address)
			{
				if (Track + Padding < ShowData.Num())
				{
					ShowData[Track + Padding] = ShowData[Track];
				}
				Track--;
			}
		}
		else
		{
			while (Address < ShowData.Num())
			{
				if (Address + Padding > 0)
				{
					ShowData[Address + Padding] = ShowData[Address];
				}
				Address++;
			}
		}
	}
}

void UShowManager::SelectAudio()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	bool bPreviousShowMouseCursor = false;

	if (PC)
	{
		bPreviousShowMouseCursor = PC->bShowMouseCursor;
		PC->bShowMouseCursor = true;
		PC->SetInputMode(FInputModeUIOnly());
	}

	if (ReferenceSpeaker)
	{
		ReferenceSpeaker->Play(0.0f);
		ReferenceSpeaker->Stop();
	}

	const TArray<FExtensionFilter> Extensions;
	FExtensionFilter AudioFilter;
	AudioFilter.Description = L"Audio Files";
	AudioFilter.Extension = L"*.wav;*.mp3;*.flac;*.ogg;*.opus;*.acc";

	if (TArray<FString> OpenFilenames = UFileDialogLibrary::OpenFilePanelMultiFilter(L"Select your audio file", L"", Extensions, false); !OpenFilenames.IsEmpty())
	{
		if (!OpenFilenames[0].IsEmpty())
		{	
			TArray<uint8> AudioFile;
			FFileHelper::LoadFileToArray(AudioFile, OpenFilenames[0].GetCharArray().GetData());
			ReferenceClip = AudioParser::FromAudioBytes(AudioFile.GetData(), AudioFile.Num(), FPaths::GetExtension(OpenFilenames[0], true), "Audio In");
			CreateShowData();
		}
	}

	if (PC)
	{
		PC->bShowMouseCursor = bPreviousShowMouseCursor;
		PC->SetInputMode(FInputModeGameOnly());
	}
}

void UShowManager::CreateShowData()
{
	ShowData.Init(BitMap(), 100);
}

void UShowManager::SaveRecording()
{
	if (!ShowData.IsEmpty())
	{
		bRecordData = false;
		bPlaybackData = false;
		
		TArray<uint8> Aud = AudioParser::FromSoundWave(ReferenceClip, ".flac");
		FileLoader::SaveShow(ShowPath[0], ShowData, Aud);
	}
}

bool UShowManager::SaveRecordingAs()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	bool bPreviousShowMouseCursor = false;

	if (PC)
	{
		bPreviousShowMouseCursor = PC->bShowMouseCursor;
		PC->bShowMouseCursor = true;
		PC->SetInputMode(FInputModeUIOnly());
	}
	
	FString SavePath = UFileDialogLibrary::SaveFilePanel(L"Save your show", L"", L"Show", ".rtr");
	if (!SavePath.IsEmpty())
	{
		TArray<uint8> AudioFile = AudioParser::FromSoundWave(ReferenceClip, ".flac");
		FileLoader::SaveShow(SavePath, ShowData, AudioFile);
	}

	if (PC)
	{
		PC->bShowMouseCursor = bPreviousShowMouseCursor;
		PC->SetInputMode(FInputModeGameOnly());
	}
	
	return true;
}

void UShowManager::GetVideos(const FString Find)
{
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	
	if (!Find.IsEmpty() && PlatformFile.FileExists(*Find))
	{
		UE_LOG(LogTemp, Warning, TEXT("File Does Not Exist: %s"), *Find);
		return;
	}
	
	FString DirectoryName = FPaths::GetPath(Find);
	const FString FileNameWithoutExtension = FPaths::GetBaseFilename(Find);
	
	const auto Video1 = FPaths::Combine(DirectoryName, FileNameWithoutExtension + ".mp4" );
	if (PlatformFile.FileExists(*Video1))
	{
		UE_LOG(LogTemp, Log, TEXT("Found Video For TV1: %s"), *Video1);
		VideoPaths[0] = Video1;
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Could not find Video For TV1: %s"), *Video1);
	}
	
	const auto Video2 = FPaths::Combine(DirectoryName, FileNameWithoutExtension + ".2.mp4" );
	if (PlatformFile.FileExists(*Video2))
	{
		UE_LOG(LogTemp, Log, TEXT("Found Video For TV2: %s"), *Video2);
		VideoPaths[1] = Video2;
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Could not find Video For TV2: %s"), *Video2);
	}
	
	const auto Video3 = FPaths::Combine(DirectoryName, FileNameWithoutExtension + ".3.mp4" );
	if (PlatformFile.FileExists(*Video3))
	{
		UE_LOG(LogTemp, Log, TEXT("Found Video For TV3: %s"), *Video3);
		VideoPaths[2] = Video3;
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Could not find Video For TV3: %s"), *Video3);
		if (!VideoPaths[1].IsEmpty())
		{
			UE_LOG(LogTemp, Log, TEXT("There is a video for TV2, TV3 will be rerouting to TV2 video"));
			VideoPaths[2] = VideoPaths[1];
		}
	}
}