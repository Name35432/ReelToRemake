#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/WidgetInteractionComponent.h"
#include "MainPlayer.generated.h"

UCLASS()
class REELTOREMAKE_API AMainPlayer : public ACharacter
{
    GENERATED_BODY()

public:
    AMainPlayer();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    UCameraComponent* CameraComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flashlight")
    USpotLightComponent* FlashlightComp;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float WalkSpeed = 500.f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float SprintSpeed = 900.f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float CrouchSpeed = 250.f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flying")
    bool bFreeCam = false;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flying")
    float FlySpeed = 1000.f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crouch")
    float StandingHeight = 88.f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crouch")
    float CrouchingHeight = 44.f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crouch")
    float CrouchTransitionSpeed = 10.f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mouse Look")
    float RotationSmoothness = 12.f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoom")
    float MinZoom = 20.f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoom")
    float MaxZoom = 90.f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoom")
    float ZoomSpeed = 50.f;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
    UWidgetInteractionComponent* WidgetInteractionComp;

    bool bSmoothingOn = true;
    bool bPaused = false;
    bool bFreeze = false;
    
private:
    bool bIsCrouching = false;
    bool bIsSprinting = false;
    
    float TargetFOV;
    float TargetCapsuleHeight;
    
    void MoveForward(float Value);
    void MoveRight(float Value);
    void Turn(float Value);
    void LookUp(float Value);
    
    void StartJump();
    void StopJump();
    void StartCrouch();
    void StopCrouch();
    void StartSprint();
    void StopSprint();
    void ToggleFlashlight();
    void TogglePause();
    void ApplyGraphicsSettings();
    void InteractPress();
    void InteractRelease();
    
protected:
    virtual void BeginPlay() override;

public: 
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};