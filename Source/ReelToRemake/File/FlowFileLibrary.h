// These are Reel To Remake assets. If you are seeing this, you are in big trouble pal!

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FlowFileLibrary.generated.h"

USTRUCT(BlueprintType)
struct FFlowAdjustment
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flow")
	FString CharName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flow")
	TArray<float> SpeedIn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flow")
	TArray<float> SpeedOut;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flow")
	TArray<float> AccelIn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flow")
	TArray<float> AccelOut;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flow")
	TArray<float> MovementDelayIn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flow")
	TArray<float> MovementDelayOut;
};

/**
 * 
 */
UCLASS()
class REELTOREMAKE_API UFlowFileLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	UFUNCTION(BlueprintCallable, Category = "Reel To Remake|Flow Files", meta = (DisplayName = "Load Flow File"))
	static bool LoadFlowFile(const FString& FilePath, TArray<FFlowAdjustment>& OutFlows, FString& OutMetadataXml, FString& OutError);
	
	UFUNCTION(BlueprintCallable, Category = "Reel To Remake|Flow Files", meta = (DisplayName = "Save Flow File"))
	static bool SaveFlowFile(const FString& FilePath, const TArray<FFlowAdjustment>& InFlows, const FString& SavedBy, FString& OutError);
};
