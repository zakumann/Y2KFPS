// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/PlayerHUD.h"
#include "Components/ProgressBar.h"

void UPlayerHUD::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(SlowMoBar))
	{
		SlowMoBar->SetPercent(1.0f);
	}
}

void UPlayerHUD::UpdateSlowMoBar(float Progress, float MaxProgress)
{
	if (IsValid(SlowMoBar))
	{
		SlowMoBar->SetPercent(Progress / MaxProgress);
	}
}
