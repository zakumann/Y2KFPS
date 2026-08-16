// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat/CombatComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/pawn.h"
#include "Net/UnrealNetWork.h"
#include "Weapon/Weapon.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "Data/WeaponData.h"
#include "Interfaces/PlayerInterface.h"

// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	TraceLength = 20000;
}

// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, Inventory);
	DOREPLIFETIME(UCombatComponent, CurrentWeapon);
	DOREPLIFETIME_CONDITION(UCombatComponent, bAiming, COND_SkipOwner);
}

void UCombatComponent::CycleWeapon()
{
	GEngine->AddOnScreenDebugMessage(
		-1,
		5.0f,
		FColor::Cyan,
		TEXT("CycleWeapon"),
		false
	);
}

void UCombatComponent::FireWeaponPressed()
{
	Local_FireWeapon();
}


void UCombatComponent::Local_FireWeapon()
{
	if (!IsValid(CurrentWeapon)) return;
	ensure(IsValid(WeaponData));

	UAnimMontage* Montage1P = WeaponData->FirstPersonMontages.FindChecked(CurrentWeapon->WeaponType).FireMontage;
	USkeletalMeshComponent* MeshFirstPerson = IPlayerInterface::Execute_GetMeshFirstPerson(GetOwner());
	if (IsValid(Montage1P) && IsValid(MeshFirstPerson))
	{
		MeshFirstPerson->GetAnimInstance()->Montage_Play(Montage1P);
	}

	FHitResult Hit;
	CurrentWeapon->WeaponTrace(Hit, TraceLength);

	Server_FireWeapon();
}

void UCombatComponent::Server_FireWeapon_Implementation()
{
	Multicast_FireWeapon();
}

void UCombatComponent::Multicast_FireWeapon_Implementation()
{
	APawn* OwningPawn = Cast<APawn>(GetOwner());
	if (OwningPawn->IsLocallyControlled())
	{
		// do locally-controlled stuff.
	}
	else
	{
		ensure(IsValid(WeaponData));

		UAnimMontage* Montage3P = WeaponData->ThirdPersonMontages.FindChecked(CurrentWeapon->WeaponType).FireMontage;
		USkeletalMeshComponent* MeshThirdPerson = IPlayerInterface::Execute_GetMeshThirdPerson(GetOwner());
		if (IsValid(Montage3P) && IsValid(MeshThirdPerson))
		{
			MeshThirdPerson->GetAnimInstance()->Montage_Play(Montage3P);
		}
	}
}


void UCombatComponent::FireWeaponReleased()
{
}

// Server_FireWeapon
	// Multicast_FireWeapon()


void UCombatComponent::ReloadWeapon()
{
	GEngine->AddOnScreenDebugMessage(
		-1,
		5.0f,
		FColor::Cyan,
		TEXT("ReloadWeapon"),
		false
	);
}

void UCombatComponent::AimPressed()
{
	Local_Aim(true);
	Server_Aim(true);
}

void UCombatComponent::AimReleased()
{
	Local_Aim(false);
	Server_Aim(false);
}


void UCombatComponent::Server_Aim_Implementation(bool bPressed)
{
	Local_Aim(bPressed);
}

void UCombatComponent::Local_Aim(bool bPressed)
{
	bAiming = bPressed;
}


void UCombatComponent::Equip(AWeapon* Weapon)
{
	CurrentWeapon = Weapon;
	CurrentWeapon->AttachToOwningPawn();
}

void UCombatComponent::SpawnInventory()
{
	if (GetOwner()->GetLocalRole() < ROLE_Authority) return;

	for (TSubclassOf<AWeapon>& WeaponClass : DefaultWeaponClasses)
	{
		AWeapon* Weapon = SpawnWeapon(WeaponClass);
		Inventory.AddUnique(Weapon);
	}

	if (Inventory.Num() > 0)
	{
		Equip(Inventory[0]);
	}
}

void UCombatComponent::DestroyInventory()
{
	for (AWeapon* Weapon : Inventory)
	{
		if (IsValid(Weapon))
		{
			Weapon->Destroy();
		}
	}
}

void UCombatComponent::OnRep_CurrentWeapon(AWeapon* LastWeapon)
{
	if (!IsValid(CurrentWeapon)) return;
	CurrentWeapon->AttachToOwningPawn();
}

AWeapon* UCombatComponent::SpawnWeapon(TSubclassOf<AWeapon> WeaponClass) const
{
	AActor* OwningActor = GetOwner();
	if (!IsValid(OwningActor)) return nullptr;
	if (OwningActor->GetLocalRole() < ROLE_Authority) return nullptr;

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Instigator = Cast<APawn>(OwningActor);
	SpawnInfo.Owner = OwningActor; 
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	return GetWorld()->SpawnActor<AWeapon>(WeaponClass, SpawnInfo);
}