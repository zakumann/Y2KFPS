// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UCLASS()
class Y2KFPS_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeapon();

	USkeletalMeshComponent* GetMeshFirstPerson() const;
	USkeletalMeshComponent* GetMeshThirdPerson() const;

protected:
	virtual void BeginPlay() override;

private:
	// Weapon Mesh: 1st person view
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> MeshFirstPerson;

	// Weapon Mesh: 3rd person view
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> MeshThirdPerson;
};
