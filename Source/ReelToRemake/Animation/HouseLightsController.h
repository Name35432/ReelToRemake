// These are Reel To Remake assets. If you are seeing this, you are in big trouble pal!

#pragma once

#include "CoreMinimal.h"
#include "BitChart.h"
#include "Components/ActorComponent.h"
#include "HouseLightsController.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class REELTOREMAKE_API UHouseLightsController : public UActorComponent
{
	GENERATED_BODY()

public:	
	UHouseLightsController();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="HouseLights")
	bool bNoBit = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="HouseLights")
	int Bit = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="HouseLights")
	bool bBottomDrawer = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="HouseLights")
	bool bNonStandard = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="HouseLights")
	float FadeSpeed = 1.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="HouseLights")
	bool bLegacyFade = false;
	
private:
	TArray<ULightComponent*> Lights;
	TArray<float> LightsOriginalBrightness;
	float NextTime = 0;
	bool bLastState = true;
	bool bHolding = false;
	ABitChart* MagnaCarta;

protected:
	virtual void BeginPlay() override;
	float GetFadeDelta(float DeltaTime) const;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category="HouseLights")
	void OnShowLoaded(bool bOverride);
	
	UFUNCTION(BlueprintCallable, Category="HouseLights")
	void OnShowUnloaded(bool bOverride);
};
