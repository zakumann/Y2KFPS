// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat/CombatComponent.h"

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
	GEngine->AddOnScreenDebugMessage(
		-1,
		5.0f,
		FColor::Cyan,
		TEXT("AimPressed"),
		false
	);
}

void UCombatComponent::AimReleased()
{
	GEngine->AddOnScreenDebugMessage(
		-1,
		5.0f,
		FColor::Cyan,
		TEXT("AimReleased"),
		false
	);
}
