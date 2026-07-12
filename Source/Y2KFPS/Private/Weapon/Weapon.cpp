// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Weapon.h"

#include "Components/SkeletalMeshComponent.h"


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

}

USkeletalMeshComponent* AWeapon::GetMeshFirstPerson() const
{
	return MeshFirstPerson;
}

USkeletalMeshComponent* AWeapon::GetMeshThirdPerson() const
{
	return MeshThirdPerson;
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

