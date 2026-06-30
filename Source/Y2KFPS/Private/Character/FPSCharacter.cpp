// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/FPSCharacter.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AFPSCharacter::AFPSCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>("FirstPersonCamera");
	FirstPersonCamera->SetupAttachment(RootComponent);
	FirstPersonCamera->bUsePawnControlRotation = true;

	FPSArm = CreateDefaultSubobject<USkeletalMeshComponent>("FPSArm");
	FPSArm->SetupAttachment(FirstPersonCamera);
	FPSArm->SetOnlyOwnerSee(true);
	FPSArm->bCastDynamicShadow = false;
	FPSArm->CastShadow = false;
}

// Called when the game starts or when spawned
void AFPSCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	GetWorld()->GetTimerManager().SetTimer(SlowMoTimer, this, &AFPSCharacter::UsingSlowMo, 0.016f, true);
	GetWorld()->GetTimerManager().PauseTimer(SlowMoTimer);

	// Add Input mapping context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		// Get local player subsystem
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// Add input context
			Subsystem->AddMappingContext(InputMapping, 0);
		}
	}
}

// Called every frame
void AFPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFPSCharacter::Move);
		EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFPSCharacter::Look);
		EnhancedInput->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AFPSCharacter::Jump);

		// SlowMotion
		EnhancedInput->BindAction(SlowMoAction, ETriggerEvent::Started, this, &AFPSCharacter::EnableSlowMo);
		EnhancedInput->BindAction(SlowMoAction, ETriggerEvent::Completed, this, &AFPSCharacter::DisableSlowMo);
	}
}

void AFPSCharacter::Move(const FInputActionValue& InputValue)
{
	FVector2D InputVector = InputValue.Get<FVector2D>();

	if (IsValid(Controller))
	{
		// Get Forward Direction
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// Add movement input
		AddMovementInput(ForwardDirection, InputVector.Y);
		AddMovementInput(RightDirection, InputVector.X);
	}
}

void AFPSCharacter::Look(const FInputActionValue & InputValue)
{
	FVector2D InputVector = InputValue.Get<FVector2D>();

	if (IsValid(Controller))
	{
		AddControllerYawInput(InputVector.X);
		AddControllerPitchInput(InputVector.Y);
	}
}

void AFPSCharacter::Jump()
{
	ACharacter::Jump();
}

void AFPSCharacter::EnableSlowMo()
{
	if (bDepletedSlowMo) return;
	bIsUsingSlowMo = true;
	bUsedSlowMo = false;
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.25);
	GetWorld()->GetTimerManager().UnPauseTimer(SlowMoTimer);
}

void AFPSCharacter::DisableSlowMo()
{
	if (!bIsUsingSlowMo) return;

	bIsUsingSlowMo = false;
	bUsedSlowMo = true;
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
	GetWorld()->GetTimerManager().UnPauseTimer(SlowMoTimer);
}

void AFPSCharacter::UsingSlowMo()
{
	UE_LOG(LogTemp, Warning, TEXT("SlowMoCount: %f"), SlowMoCount);
	if (bIsUsingSlowMo)
	{
		SlowMoCount = FMath::Max(SlowMoCount - 1, 0);
		if (SlowMoCount <= 0)
		{
			bDepletedSlowMo = true;
			bUsedSlowMo = true;
			bIsUsingSlowMo = false;
			UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
		}
	}
	else if (bUsedSlowMo)
	{
		SlowMoCount = FMath::Min(SlowMoCount + RechargeRate, 100);
		if (SlowMoCount > 10)
		{
			bDepletedSlowMo = false;
		}
		if (SlowMoCount >= 100)
		{
			bUsedSlowMo = false;
			bDepletedSlowMo = false;
			SlowMoCount = 100;
			GetWorld()->GetTimerManager().PauseTimer(SlowMoTimer);
		}
	}
}

