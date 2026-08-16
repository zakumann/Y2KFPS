// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Weapon.h"

#include "KismetTraceUtils.h"
#include "Components/SkeletalMeshComponent.h"
#include "Y2KFPS/Y2KFPS.h"
#include "GameFramework/Pawn.h"
#include "Interfaces/PlayerInterface.h"
#include "Kismet/KismetMathLibrary.h"


AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	bNetUseOwnerRelevancy = true;

	MeshFirstPerson = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshFirstPerson"));
	MeshFirstPerson->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	MeshFirstPerson->bReceivesDecals = false;
	MeshFirstPerson->CastShadow = false;
	MeshFirstPerson->SetHiddenInGame(true);
	SetRootComponent(MeshFirstPerson);

	MeshThirdPerson = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshThirdPerson"));
	MeshThirdPerson->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	MeshThirdPerson->bReceivesDecals = false;
	MeshThirdPerson->CastShadow = true;
	MeshThirdPerson->SetupAttachment(MeshFirstPerson);
	MeshThirdPerson->SetHiddenInGame(true);

	AimFieldOfView = 65.0f;
	TraceRadius = 5.0f;
}

void AWeapon::OnRep_Instigator()
{
	Super::OnRep_Instigator();
	AttachToOwningPawn();
}

USkeletalMeshComponent* AWeapon::GetMeshFirstPerson() const
{
	return MeshFirstPerson;
}

USkeletalMeshComponent* AWeapon::GetMeshThirdPerson() const
{
	return MeshThirdPerson;
}

void AWeapon::AttachToOwningPawn() const
{
	APawn* OwningPawn = GetInstigator();
	if (!IsValid(OwningPawn) || !OwningPawn->Implements<UPlayerInterface>()) return;

	SetMeshVisibilities(OwningPawn);

	const FName AttachPoint = IPlayerInterface::Execute_GetWeaponAttachPoint(OwningPawn, WeaponType);
	USkeletalMeshComponent* PawnMeshFirstPerson = IPlayerInterface::Execute_GetMeshFirstPerson(OwningPawn);
	USkeletalMeshComponent* PawnMeshThirdPerson = IPlayerInterface::Execute_GetMeshThirdPerson(OwningPawn);

	MeshFirstPerson->AttachToComponent(PawnMeshFirstPerson, FAttachmentTransformRules::KeepRelativeTransform, AttachPoint);
	MeshThirdPerson->AttachToComponent(PawnMeshThirdPerson, FAttachmentTransformRules::KeepRelativeTransform, AttachPoint);
}

void AWeapon::WeaponTrace(FHitResult& OutHit, float TraceLength)
{
	FCollisionQueryParams QueryParams;
	QueryParams.bReturnPhysicalMaterial = true;
	QueryParams.AddIgnoredActor(GetOwner());

	FCollisionResponseParams ResponseParams;
	ResponseParams.CollisionResponse.SetAllChannels(ECR_Ignore);
	ResponseParams.CollisionResponse.SetResponse(ECC_Pawn, ECR_Block);
	ResponseParams.CollisionResponse.SetResponse(ECC_WorldStatic, ECR_Block);
	ResponseParams.CollisionResponse.SetResponse(ECC_WorldDynamic, ECR_Block);
	ResponseParams.CollisionResponse.SetResponse(ECC_PhysicsBody, ECR_Block);

	ensure(GetInstigator());
	if (APlayerController* PC = Cast<APlayerController>(GetInstigator()->GetController()); IsValid(PC))
	{
		FVector EyesWorldLocation;
		FRotator EyesWorldRotation;
		PC->GetActorEyesViewPoint(EyesWorldLocation, EyesWorldRotation);
		const FVector EyesWorldDirection = UKismetMathLibrary::GetForwardVector(EyesWorldRotation);

		const FVector Start = EyesWorldLocation;
		const FVector End = Start + EyesWorldDirection * TraceLength;

		const bool bHit = GetWorld()->SweepSingleByChannel(
			OutHit,
			Start,
			End,
			FQuat::Identity,
			FPSTraceChannels::ECC_Weapon,
			FCollisionShape::MakeSphere(TraceRadius),
			QueryParams,
			ResponseParams);

		DrawDebugSphereTraceSingle(
			GetWorld(),
			Start,
			End,
			TraceRadius,
			EDrawDebugTrace::ForDuration,
			bHit,
			OutHit,
			FColor::Green,
			FColor::Red,
			5.0f);
	}
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWeapon::SetMeshVisibilities(APawn* OwningPawn) const
{
	if (OwningPawn->IsLocallyControlled())
	{
		MeshFirstPerson->SetHiddenInGame(false);
		MeshThirdPerson->SetHiddenInGame(true);
	}
	else
	{
		MeshFirstPerson->SetHiddenInGame(true);
		MeshThirdPerson->SetHiddenInGame(false);
	}
}

