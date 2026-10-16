#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BitChart.h"
#include "CharacterMovements.generated.h"

class USkeletalMeshComponent;
class UAnimInstance;
class FNumericProperty;

USTRUCT(BlueprintType)
struct FBits
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Bit = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bBottomDrawer = false;

    // Runtime-only reflection pointer.
    FDoubleProperty* FloatProperty = nullptr;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class REELTOREMAKE_API UCharacterMovements : public UActorComponent
{
    GENERATED_BODY()

public:
    UCharacterMovements();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="General Settings")
    bool bUseDp = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="General Settings")
    int32 DualPressureBit = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="General Settings")
    float PsiScale = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="General Settings")
    TArray<float> SpeedIn;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="General Settings")
    TArray<float> SpeedOut;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="General Settings")
    TArray<float> AccelerationIn;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="General Settings")
    TArray<float> AccelerationOut;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="General Settings")
    bool bMovementDelayEnable = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="General Settings")
    TArray<float> MovementDelayIn;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="General Settings")
    TArray<float> MovementDelayOut;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Debug")
    ABitChart* BitChart = nullptr;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Character Variant")
    TObjectPtr<USkeletalMeshComponent> Mesh = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Character Variant")
    TObjectPtr<UAnimInstance> AnimInstance = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Debug")
    TArray<float> CharacterValves;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Debug")
    TArray<float> MovementDelayTimers;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Debug")
    TArray<bool> LastBitState;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Debug")
    TArray<float> HeavyChartOut;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Debug")
    TArray<float> HeavyChartIn;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Debug")
    TArray<FBits> Bits;
    
    UFUNCTION(BlueprintCallable, Category="Character Variant")
    void SetMovementMesh(USkeletalMeshComponent* NewMesh);
    
    UFUNCTION(BlueprintCallable, Category="Character Variant")
    void RebindAnimationVariables();

    virtual void BeginPlay() override;

    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    void UpdateMovements(float DeltaTime);

protected:
    float Scale = 0.f;
};