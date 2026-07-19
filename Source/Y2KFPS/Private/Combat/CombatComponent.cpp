// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat/CombatComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/pawn.h"
#include "Net/UnrealNetWork.h"
#include "Weapon/Weapon.h"

// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

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

	GEngine->AddOnScreenDebugMessage(
		-1,
		5.0f,
		FColor::Cyan,
		TEXT("FireWeaponPressed"),
		false
	);
}

void UCombatComponent::FireWeaponReleased()
{
	GEngine->AddOnScreenDebugMessage(
		-1,
		5.0f,
		FColor::Cyan,
		TEXT("FireWeaponReleased"),
		false
	);
}

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