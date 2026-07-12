// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/FPSCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Combat/CombatComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

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
}

// Called when the game starts or when spawned
void AFPSCharacter::BeginPlay()
{
	Super::BeginPlay();
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

	UEnhancedInputComponent* ShooterInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	ShooterInputComponent->BindAction(CycleWeaponAction, ETriggerEvent::Started, this, &AFPSCharacter::CycleWeapon);
	ShooterInputComponent->BindAction(FireWeaponAction, ETriggerEvent::Started, this, &AFPSCharacter::FireWeaponPressed);
	ShooterInputComponent->BindAction(FireWeaponAction, ETriggerEvent::Completed, this, &AFPSCharacter::FireWeaponReleased);
	ShooterInputComponent->BindAction(AimWeaponAction, ETriggerEvent::Started, this, &AFPSCharacter::AimPressed);
	ShooterInputComponent->BindAction(AimWeaponAction, ETriggerEvent::Completed, this, &AFPSCharacter::AimReleased);
	ShooterInputComponent->BindAction(ReloadWeaponAction, ETriggerEvent::Started, this, &AFPSCharacter::ReloadWeapon);
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
}

void AFPSCharacter::AimReleased()
{
	Combat->AimReleased();
}
