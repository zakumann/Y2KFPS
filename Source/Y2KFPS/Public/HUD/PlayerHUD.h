// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUD.generated.h"

class UProgressBar;

/**
 * 
 */
UCLASS()
class Y2KFPS_API UPlayerHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void UpdateSlowMoBar(float Progress, float MaxProgress);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> SlowMoBar;
};
