// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/PlayerInterface.h"
#include "FPSCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class USkeletalMeshComponent;
class UCombatComponent;
class UInputAction;

UCLASS()
class Y2KFPS_API AFPSCharacter : public ACharacter, public IPlayerInterface
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FirstPersonCamera;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> SpringArm;

	/** First person arms skeletal mesh (visible only to the local controlling player) */
	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	TObjectPtr<USkeletalMeshComponent> FPSArm;

	UPROPERTY(EditAnywhere, Category = "FPS|EnhanceInput")
	TObjectPtr<UInputAction> CycleWeaponAction;

	UPROPERTY(EditAnywhere, Category = "FPS|EnhanceInput")
	TObjectPtr<UInputAction> FireWeaponAction;

	UPROPERTY(EditAnywhere, Category = "FPS|EnhanceInput")
	TObjectPtr<UInputAction> ReloadWeaponAction;

	UPROPERTY(EditAnywhere, Category = "FPS|EnhanceInput")
	TObjectPtr<UInputAction> AimWeaponAction;
public:
	// Sets default values for this character's properties
	AFPSCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;

	/** PlayerInterface*/
	virtual FName GetWeaponAttachPoint_Implementation(const FGameplayTag& WeaponType) const override;
	virtual USkeletalMeshComponent* GetMeshFirstPerson_Implementation() const override;
	virtual USkeletalMeshComponent* GetMeshThirdPerson_Implementation() const override;

	/** ~PlayerInterface*/
private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCombatComponent> Combat;


	void CycleWeapon();
	void ReloadWeapon();
	void FireWeaponPressed();
	void FireWeaponReleased();
	void AimPressed();
	void AimReleased();
};
