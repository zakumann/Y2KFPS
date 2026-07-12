// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FPSPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class UPlayerHUD;

/**
 * 
 */
UCLASS()
class Y2KFPS_API AFPSPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AFPSPlayerController();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
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

	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	TObjectPtr<UInputAction> SprintAction;

	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	TObjectPtr<UInputAction> CrouchAction;

	void Move(const FInputActionValue& InputActionValue);
	void Look(const FInputActionValue& InputActionValue);
	void Crouch();
	void Jump();
	void Sprint();
	void StopSprint();

protected:

	// First-person primitives field of view
	UPROPERTY(EditAnywhere, Category = "Camera")
	float FirstPersonFieldofView = 70.0f;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float FirstPersonScale = 0.6f;

	UFUNCTION(BlueprintCallable, Category = "Input")
	void ToggleSlowMo();

	UFUNCTION(BlueprintCallable, Category = "Input")
	void EnableSlowMo();

	UFUNCTION(BlueprintCallable, Category = "Input")
	void DisableSlowMo();

	UPROPERTY()
	FTimerHandle SlowMoTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SlowMoCount = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RechargeRate = 0.025f;

	UFUNCTION()
	void UsingSlowMo();

	UPROPERTY()
	bool bUsedSlowMo = false;

	UPROPERTY()
	bool bIsUsingSlowMo = false;

	UPROPERTY()
	bool bDepletedSlowMo = false;

	UPROPERTY()
	TObjectPtr<UPlayerHUD> PlayerHUD;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UPlayerHUD> PlayerHUDClass;

	UPROPERTY()
	TObjectPtr<AFPSPlayerController> PC;
};
