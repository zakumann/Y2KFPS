// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ShooterTypes.generated.h"

UENUM(BlueprintType)
enum class ETurningInPlace : uint8
{
	Left		UMETA(DisplayName = "TurningLeft"),
	Right		UMETA(DisplayName = "TurningRight"),
	NotTurning	UMETA(DisplayName = "NotTurning")
};
