// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Weapon.h"

#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Pawn.h"
#include "Interfaces/PlayerInterface.h"


AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	bNetUseOwnerRelevancy = true;

	MeshFirstPerson = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshFirstPerson"));
	MeshFirstPerson->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	MeshFirstPerson->bReceivesDecals = false;
	MeshFirstPerson->CastShadow = false;
	//MeshFirstPerson->SetHiddenInGame(true);
	SetRootComponent(MeshFirstPerson);

	MeshThirdPerson = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshThirdPerson"));
	MeshThirdPerson->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	MeshThirdPerson->bReceivesDecals = false;
	MeshThirdPerson->CastShadow = true;
	MeshThirdPerson->SetupAttachment(MeshFirstPerson);
	//MeshThirdPerson->SetHiddenInGame(true);

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

