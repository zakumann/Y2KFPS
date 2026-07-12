// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController/FPSPlayerController.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Engine/LocalPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "HUD/PlayerHUD.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"

AFPSPlayerController::AFPSPlayerController()
{
	bReplicates = true;
}

void AFPSPlayerController::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(SlowMoTimer, this, &AFPSPlayerController::UsingSlowMo, 0.016f, true);
	GetWorld()->GetTimerManager().PauseTimer(SlowMoTimer);

	if (IsValid(PlayerHUDClass) && IsValid(this))
	{
		PlayerHUD = CreateWidget<UPlayerHUD>(this, PlayerHUDClass);
		if (IsValid(PlayerHUD))
		{
			PlayerHUD->AddToViewport();
		}
	}

	// Add Input mapping context.
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (IsValid(Subsystem))
	{
		// Add input context
		Subsystem->AddMappingContext(InputMapping, 0);
	}
}

void AFPSPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if(UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFPSPlayerController::Move);
		EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFPSPlayerController::Look);
		EnhancedInput->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AFPSPlayerController::Jump);
	

		// SlowMotion
		EnhancedInput->BindAction(SlowMoAction, ETriggerEvent::Started, this, &AFPSPlayerController::ToggleSlowMo);
		// EnhancedInput->BindAction(SlowMoAction, ETriggerEvent::Started, this, &AFPSPlayerController::EnableSlowMo);
		// EnhancedInput->BindAction(SlowMoAction, ETriggerEvent::Completed, this, &AFPSPlayerController::DisableSlowMo);

		// Sprint
		EnhancedInput->BindAction(SprintAction, ETriggerEvent::Started, this, &AFPSPlayerController::Sprint);
		EnhancedInput->BindAction(SprintAction, ETriggerEvent::Completed, this, &AFPSPlayerController::StopSprint);

		// Crouch
		EnhancedInput->BindAction(CrouchAction, ETriggerEvent::Started, this, &AFPSPlayerController::Crouch);
	}
}

void AFPSPlayerController::Move(const FInputActionValue& InputActionValue)
{
	FVector2D InputVector = InputActionValue.Get<FVector2D>();

	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDireciton = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (APawn* ControlledPawn = GetPawn())
	{
		ControlledPawn->AddMovementInput(ForwardDirection, InputVector.Y);
		ControlledPawn->AddMovementInput(RightDireciton, InputVector.X);
	}
}

void AFPSPlayerController::Look(const FInputActionValue& InputActionValue)
{
	const FVector2D InputVector = InputActionValue.Get<FVector2D>();

	AddYawInput(InputVector.X);
	AddPitchInput(InputVector.Y);
}

void AFPSPlayerController::Jump()
{
	if (!IsValid(GetCharacter())) return;
	UCharacterMovementComponent* CMC = GetCharacter()->GetCharacterMovement();
	if (!IsValid(CMC)) return;

	if (CMC->bWantsToCrouch)
	{
		CMC->bWantsToCrouch = false;
	}
	else
	{
		GetCharacter()->Jump();
	}
}

void AFPSPlayerController::Crouch()
{
	if (!IsValid(GetCharacter())) return;

	if (UCharacterMovementComponent* CMC = GetCharacter()->GetCharacterMovement(); IsValid(CMC))
	{
		CMC->bWantsToCrouch = (CMC->bWantsToCrouch == 0);
	}
}

void AFPSPlayerController::Sprint()
{
	if (!IsValid(GetCharacter())) return;
	UCharacterMovementComponent* CMC = GetCharacter()->GetCharacterMovement();
	if (!IsValid(CMC)) return;

	CMC->MaxWalkSpeed = 900.0f;
}

void AFPSPlayerController::StopSprint()
{
	if (!IsValid(GetCharacter())) return;
	UCharacterMovementComponent* CMC = GetCharacter()->GetCharacterMovement();
	if (!IsValid(CMC)) return;

	CMC->MaxWalkSpeed = 600.0f;
}

void AFPSPlayerController::ToggleSlowMo()
{
	if (bIsUsingSlowMo)
	{
		DisableSlowMo();
	}
	else
	{
		EnableSlowMo();
	}
}

void AFPSPlayerController::EnableSlowMo()
{
	if (bDepletedSlowMo) return;
	bIsUsingSlowMo = true;
	bUsedSlowMo = false;
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.25);
	GetWorld()->GetTimerManager().UnPauseTimer(SlowMoTimer);
}

void AFPSPlayerController::DisableSlowMo()
{
	if (!bIsUsingSlowMo) return;

	bIsUsingSlowMo = false;
	bUsedSlowMo = true;
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
	GetWorld()->GetTimerManager().UnPauseTimer(SlowMoTimer);
}

void AFPSPlayerController::UsingSlowMo()
{
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
	PlayerHUD->UpdateSlowMoBar(SlowMoCount, 100);
}