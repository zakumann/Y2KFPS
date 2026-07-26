// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/FPSCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Combat/CombatComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Data/WeaponData.h"
#include "Kismet/KismetMathLibrary.h"
#include "Weapon/Weapon.h"

// Sets default values
AFPSCharacter::AFPSCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 0.0f;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 15.0f;
	SpringArm->bUsePawnControlRotation = true;

	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>("FirstPersonCamera");
	FirstPersonCamera->SetupAttachment(SpringArm);
	FirstPersonCamera->bUsePawnControlRotation = false;

	FPSArm = CreateDefaultSubobject<USkeletalMeshComponent>("FPSArm");
	FPSArm->SetupAttachment(FirstPersonCamera);
	FPSArm->bOnlyOwnerSee = true;
	FPSArm->bOwnerNoSee = false;
	FPSArm->bCastDynamicShadow = false;
	FPSArm->bReceivesDecals = false;
	FPSArm->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	FPSArm->PrimaryComponentTick.TickGroup = TG_PrePhysics;

	GetMesh()->bOnlyOwnerSee = false;
	GetMesh()->bOwnerNoSee = true;
	GetMesh()->bReceivesDecals = false;

	Combat = CreateDefaultSubobject<UCombatComponent>("Combat");
	Combat->SetIsReplicated(true);

	DefaultFieldOfView = 90.0f;
	TurningStatus = ETurningInPlace::NotTurning;
}

// Called when the game starts or when spawned
void AFPSCharacter::BeginPlay()
{
	Super::BeginPlay();

	FirstPersonCamera->SetFieldOfView(DefaultFieldOfView);

	StartingAimRotation = FRotator(0.0f, GetBaseAimRotation().Yaw, 0.0f);

}

void AFPSCharacter::BeginDestroy()
{
	Super::BeginDestroy();

	if (IsValid(Combat))
	{
		Combat->DestroyInventory();
	}
}

FRotator AFPSCharacter::GetFixedAnimRotation() const
{
	FRotator AimRotation = GetBaseAimRotation();
	if (AimRotation.Pitch > 90.0f && !IsLocallyControlled())
	{
		// map pitch from (270, 360) to (-90, 0)
		const FVector2D InRange(270.0f, 360.0f);
		const FVector2D OutRange(-90.0f, 0.0f);
		AimRotation.Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AimRotation.Pitch);
	}

	return AimRotation;
}

bool AFPSCharacter::HasCurrentWeapon() const
{

	return IsValid(Combat) && Combat->CurrentWeapon != nullptr;
}

// Called every frame
void AFPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CalculateTurnInPlaceParameters(DeltaTime);
	CalculateFABRIKSocketTransform();
}

void AFPSCharacter::CalculateTurnInPlaceParameters(float DeltaTime)
{
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	float Speed = Velocity.Size();
	bool bIsInAir = GetCharacterMovement()->IsFalling();

	if (Speed == 0.0f && !bIsInAir) // standing still, not jumping
	{
		FRotator CurrentAimRotation(0.0f, GetBaseAimRotation().Yaw, 0.0f);
		// StartingAimRotation initially set in BeginPlay
		FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation, StartingAimRotation);
		AO_Yaw = DeltaAimRotation.Yaw;

		if (TurningStatus == ETurningInPlace::NotTurning)
		{
			InterpAO_Yaw = AO_Yaw;
		}

		TurnInPlace(DeltaTime); //interpolates the InterpAO_Yaw value to zero.
	}

	if (Speed > 0.0f || bIsInAir)
	{
		StartingAimRotation = FRotator(0.0f, GetBaseAimRotation().Yaw, 0.0f);
		AO_Yaw = 0.0f;

		FRotator AimRotation = GetBaseAimRotation();
		FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(GetVelocity());
		MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;
		TurningStatus = ETurningInPlace::NotTurning;
	}

	AO_Yaw *= -1.0f;
}

void AFPSCharacter::TurnInPlace(float DeltaTime)
{
	if (AO_Yaw > 90.0f)
	{
		TurningStatus = ETurningInPlace::Right;
	}
	else if (AO_Yaw < -90.0f)
	{
		TurningStatus = ETurningInPlace::Left;
	}
	if (TurningStatus != ETurningInPlace::NotTurning) // we are turning 
	{
		InterpAO_Yaw = FMath::FInterpTo(InterpAO_Yaw, 0.0f, DeltaTime, 4.0f);
		AO_Yaw = InterpAO_Yaw;
		if (FMath::Abs(AO_Yaw) < 5.0f)
		{
			TurningStatus = ETurningInPlace::NotTurning;
			StartingAimRotation = FRotator(0.0f, GetBaseAimRotation().Yaw, 0.0f);
		}
	}
}

void AFPSCharacter::CalculateFABRIKSocketTransform()
{
	if (IsValid(Combat) && IsValid(Combat->CurrentWeapon) && IsValid(Combat->CurrentWeapon->GetMeshThirdPerson()))
	{
		FABRIK_SocketTransform = Combat->CurrentWeapon->GetMeshThirdPerson()->GetSocketTransform("FABRIK_Socket", RTS_World);

		FVector OutLocation;
		FRotator OutRotation;
		GetMesh()->TransformToBoneSpace(
			"hand_r",
			FABRIK_SocketTransform.GetLocation(),
			FABRIK_SocketTransform.GetRotation().Rotator(),
			OutLocation,
			OutRotation);
		FABRIK_SocketTransform.SetLocation(OutLocation);
		FABRIK_SocketTransform.SetRotation(OutRotation.Quaternion());
	}
}

// Called to bind functionality to input
void AFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* ShooterInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	ShooterInputComponent->BindAction(CycleWeaponAction, ETriggerEvent::Started, this, &AFPSCharacter::CycleWeapon);
	ShooterInputComponent->BindAction(FireWeaponAction, ETriggerEvent::Started, this, &AFPSCharacter::FireWeaponPressed);
	ShooterInputComponent->BindAction(FireWeaponAction, ETriggerEvent::Completed, this, &AFPSCharacter::FireWeaponReleased);
	ShooterInputComponent->BindAction(AimWeaponAction, ETriggerEvent::Started, this, &AFPSCharacter::AimPressed);
	ShooterInputComponent->BindAction(AimWeaponAction, ETriggerEvent::Completed, this, &AFPSCharacter::AimReleased);
	ShooterInputComponent->BindAction(ReloadWeaponAction, ETriggerEvent::Started, this, &AFPSCharacter::ReloadWeapon);
}

void AFPSCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (IsValid(Combat))
	{
		Combat->SpawnInventory();
	}
}

FName AFPSCharacter::GetWeaponAttachPoint_Implementation(const FGameplayTag& WeaponType) const
{
	checkf(Combat->WeaponData, TEXT("No Weapon Data Asset - Please fill out BP_ShooterCharacter"));
	return Combat->WeaponData->GripPoints.FindChecked(WeaponType);
}

USkeletalMeshComponent* AFPSCharacter::GetMeshFirstPerson_Implementation() const
{
	return FPSArm;
}

USkeletalMeshComponent* AFPSCharacter::GetMeshThirdPerson_Implementation() const
{
	return GetMesh();
}

void AFPSCharacter::CycleWeapon()
{
	Combat->CycleWeapon();
}

void AFPSCharacter::ReloadWeapon()
{
	Combat->ReloadWeapon();
}

void AFPSCharacter::FireWeaponPressed()
{
	Combat->FireWeaponPressed();
}

void AFPSCharacter::FireWeaponReleased()
{
	Combat->FireWeaponReleased();
}

void AFPSCharacter::AimPressed()
{
	Combat->AimPressed();
	OnAim(true);
}

void AFPSCharacter::AimReleased()
{
	Combat->AimReleased();
	OnAim(false);
}