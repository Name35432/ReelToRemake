// These are Reel To Remake assets. If you are seeing this, you are in big trouble pal!

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FileBrowser.generated.h"

USTRUCT(BlueprintType)
struct FExtensionFilter
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "File Dialog")
	FString Description; // e.g., "Audio Files"

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "File Dialog")
	FString Extension;   // e.g., "*.wav;*.flac"
};

UCLASS()
class REELTOREMAKE_API UFileDialogLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "File Dialogs")
	static TArray<FString> OpenFilePanel(const FString& Title, const FString& Directory, const FString& Extension, bool bMultiselect);
	
	UFUNCTION(BlueprintCallable, Category = "File Dialogs")
	static TArray<FString> OpenFilePanelMultiFilter(const FString& Title, const FString& Directory, const TArray<FExtensionFilter>& Extensions, bool bMultiselect);
	
	UFUNCTION(BlueprintCallable, Category = "File Dialogs")
	static FString SaveFilePanel(const FString& Title, const FString& Directory, const FString& DefaultName, const FString& Extension);
	
	UFUNCTION(BlueprintCallable, Category = "File Dialogs")
	static TArray<FString> OpenFolderPanel(const FString& Title, const FString& Directory, bool bMultiselect);
};
