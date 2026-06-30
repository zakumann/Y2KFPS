// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "FPSCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class UCameraComponent;
class USkeletalMeshComponent;

UCLASS()
class Y2KFPS_API AFPSCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FirstPersonCamera;

	/** First person arms skeletal mesh (visible only to the local controlling player) */
	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	TObjectPtr<USkeletalMeshComponent> FPSArm;
public:
	// Sets default values for this character's properties
	AFPSCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	TObjectPtr<UInputMappingContext> InputMapping;

	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	TObjectPtr<UInputAction> SlowMoAction;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	void Move(const FInputActionValue& InputValue);
	void Look(const FInputActionValue& InputValue);
	void Jump();

	UFUNCTION(BlueprintCallable, Category= "Input")
	void EnableSlowMo();

	UFUNCTION(BlueprintCallable, Category= "Input")
	void DisableSlowMo();

	UPROPERTY()
	FTimerHandle SlowMoTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SlowMoCount = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RechargeRate = 0.1f;

	UFUNCTION()
	void UsingSlowMo();

	UPROPERTY()
	bool bUsedSlowMo = false;

	UPROPERTY()
	bool bIsUsingSlowMo = false;

	UPROPERTY()
	bool bDepletedSlowMo = false;
};
