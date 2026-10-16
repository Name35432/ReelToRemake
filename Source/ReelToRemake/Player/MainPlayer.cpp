#include "MainPlayer.h"
 #include "GameFramework/CharacterMovementComponent.h"
 #include "Components/CapsuleComponent.h"
 #include "Components/WidgetInteractionComponent.h"
 #include "Math/UnrealMathUtility.h"
 
 AMainPlayer::AMainPlayer()
 {
     PrimaryActorTick.bCanEverTick = true;
 
     // Create and attach the camera
     CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
     CameraComp->SetupAttachment(GetCapsuleComponent());
     CameraComp->SetRelativeLocation(FVector(0.f, 0.f, 60.f)); // Standing Cam Offset
     CameraComp->bUsePawnControlRotation = true; // Let the controller handle mouse look
     
     WidgetInteractionComp = CreateDefaultSubobject<UWidgetInteractionComponent>(TEXT("WidgetInteractionComp"));
     WidgetInteractionComp->SetupAttachment(CameraComp);

     WidgetInteractionComp->InteractionDistance = 1000.f;
     WidgetInteractionComp->InteractionSource = EWidgetInteractionSource::World;

     WidgetInteractionComp->bShowDebug = false;

     // Make sure the interaction component is actually performing its trace
     WidgetInteractionComp->bEnableHitTesting = true;

     // Use Visibility unless you specifically configured another trace channel
     WidgetInteractionComp->TraceChannel = ECC_Visibility;

     // These are normally fine at 0, but explicitly setting them avoids
     // weirdness if you have multiple interaction components/users.
     WidgetInteractionComp->VirtualUserIndex = 0;
     WidgetInteractionComp->PointerIndex = 0;
     
     FlashlightComp = CreateDefaultSubobject<USpotLightComponent>(TEXT("FlashlightComp"));
     FlashlightComp->SetupAttachment(CameraComp);
     FlashlightComp->SetActive(false);
     
     GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
     GetCharacterMovement()->JumpZVelocity = 600.f;
     GetCharacterMovement()->AirControl = 0.2f;
 
     TargetFOV = MaxZoom;
     TargetCapsuleHeight = StandingHeight;
 }
 
 void AMainPlayer::BeginPlay()
 {
     Super::BeginPlay();
 
     // Map Unity Graphics Settings to Unreal Console Variables
     ApplyGraphicsSettings();
 
     TargetFOV = CameraComp->FieldOfView;
 }
 
 void AMainPlayer::Tick(float DeltaTime)
 {
     Super::Tick(DeltaTime);
 
     if (bPaused || bFreeze) return;
 
     // --- Smooth Crouch Transition ---
     float CurrentHeight = GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
     if (!FMath::IsNearlyEqual(CurrentHeight, TargetCapsuleHeight, 0.1f))
     {
         float NewHeight = FMath::FInterpTo(CurrentHeight, TargetCapsuleHeight, DeltaTime, CrouchTransitionSpeed);
         GetCapsuleComponent()->SetCapsuleSize(GetCapsuleComponent()->GetUnscaledCapsuleRadius(), NewHeight);
         
         // Adjust camera to match crouch
         float CamZ = FMath::GetMappedRangeValueClamped(FVector2D(CrouchingHeight, StandingHeight), FVector2D(30.f, 60.f), NewHeight);
         CameraComp->SetRelativeLocation(FVector(0.f, 0.f, CamZ));
     }
 
     // --- Smooth Zoom ---
     if (!FMath::IsNearlyEqual(CameraComp->FieldOfView, TargetFOV, 0.1f))
     {
         CameraComp->FieldOfView = FMath::FInterpTo(CameraComp->FieldOfView, TargetFOV, DeltaTime, 5.f);
     }
 
     // --- Speed Handling ---
     if (bIsCrouching)
     {
         GetCharacterMovement()->MaxWalkSpeed = CrouchSpeed;
     }
     else
     {
         GetCharacterMovement()->MaxWalkSpeed = bIsSprinting ? SprintSpeed : WalkSpeed;
     }
 }
 
 void AMainPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
 {
     Super::SetupPlayerInputComponent(PlayerInputComponent);
 
     // Bind Axis
     PlayerInputComponent->BindAxis("MoveForward", this, &AMainPlayer::MoveForward);
     PlayerInputComponent->BindAxis("MoveRight", this, &AMainPlayer::MoveRight);
     PlayerInputComponent->BindAxis("Turn", this, &AMainPlayer::Turn);
     PlayerInputComponent->BindAxis("LookUp", this, &AMainPlayer::LookUp);
 
     // Bind Actions
     PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMainPlayer::StartJump);
     PlayerInputComponent->BindAction("Jump", IE_Released, this, &AMainPlayer::StopJump);
     PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AMainPlayer::StartCrouch);
     PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AMainPlayer::StopCrouch);
     PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AMainPlayer::StartSprint);
     PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AMainPlayer::StopSprint);
     PlayerInputComponent->BindAction("Flashlight", IE_Pressed, this, &AMainPlayer::ToggleFlashlight);
     PlayerInputComponent->BindAction("Pause", IE_Pressed, this, &AMainPlayer::TogglePause);
     
     PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AMainPlayer::InteractPress);
     PlayerInputComponent->BindAction("Interact", IE_Released, this, &AMainPlayer::InteractRelease);
 }
 
 void AMainPlayer::MoveForward(float Value)
 {
     if (Value == 0.0f || bFreeze || bPaused)
         return;
 
     if (!Controller)
         return;
 
     const FRotator ControlRotation = Controller->GetControlRotation();
 
     // Ignore pitch so looking up/down doesn't make you fly forward.
     const FRotator YawRotation(0.0f, ControlRotation.Yaw, 0.0f);
 
     const FVector ForwardDirection =
         FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
 
     AddMovementInput(ForwardDirection, Value);
 }
 
 void AMainPlayer::MoveRight(float Value)
 {
     if (Value == 0.0f || bFreeze || bPaused)
         return;
 
     if (!Controller)
         return;
 
     const FRotator ControlRotation = Controller->GetControlRotation();
 
     const FRotator YawRotation(0.0f, ControlRotation.Yaw, 0.0f);
 
     const FVector RightDirection =
         FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
 
     AddMovementInput(RightDirection, Value);
 }
 
 void AMainPlayer::Turn(float Value)
 {
     if (Value == 0.0f || bFreeze || bPaused)
         return;
 
     AddControllerYawInput(Value);
 }
 
 void AMainPlayer::LookUp(float Value)
 {
     if (Value == 0.0f || bFreeze || bPaused)
         return;
 
     // Negative makes normal mouse behavior if your mapping is +1.
     AddControllerPitchInput(Value);
 }
 
 void AMainPlayer::StartJump() { if (!bIsCrouching) Jump(); }
 void AMainPlayer::StopJump() { StopJumping(); }
 
 void AMainPlayer::StartCrouch() 
 { 
     bIsCrouching = true; 
     TargetCapsuleHeight = CrouchingHeight;
 }
 
 void AMainPlayer::StopCrouch() 
 { 
     bIsCrouching = false; 
     TargetCapsuleHeight = StandingHeight;
 }
 
 void AMainPlayer::StartSprint() { bIsSprinting = true; }
 void AMainPlayer::StopSprint() { bIsSprinting = false; }
 
 void AMainPlayer::ToggleFlashlight()
 {
     FlashlightComp->SetVisibility(!FlashlightComp->IsVisible());
 }
 
 void AMainPlayer::TogglePause()
 {
     bPaused = !bPaused;
     
     APlayerController* PC = Cast<APlayerController>(GetController());
     if (PC)
     {
         PC->bShowMouseCursor = bPaused;
         PC->bEnableClickEvents = bPaused;
         PC->bEnableMouseOverEvents = bPaused;
         
         if (bPaused)
         {
             PC->SetInputMode(FInputModeGameAndUI());
         }
         else
         {
             PC->SetInputMode(FInputModeGameOnly());
         }
     }
 }
 
 void AMainPlayer::ApplyGraphicsSettings()
 {
     return;
 }
 
void AMainPlayer::InteractPress()
 {
     if (!WidgetInteractionComp)
         return;

     UE_LOG(LogTemp, Warning, TEXT("Interact Pressed"));

     UE_LOG(
         LogTemp,
         Warning,
         TEXT("IsOverHitTestVisibleWidget: %s"),
         WidgetInteractionComp->IsOverHitTestVisibleWidget() ? TEXT("YES") : TEXT("NO")
     );

     UE_LOG(
         LogTemp,
         Warning,
         TEXT("IsOverFocusableWidget: %s"),
         WidgetInteractionComp->IsOverFocusableWidget() ? TEXT("YES") : TEXT("NO")
     );

     WidgetInteractionComp->PressPointerKey(EKeys::LeftMouseButton);
 }
 
 void AMainPlayer::InteractRelease()
 {
     if (WidgetInteractionComp)
     {
         // Simulates a Left Mouse Button click release
         WidgetInteractionComp->ReleasePointerKey(EKeys::LeftMouseButton);
     }
 }