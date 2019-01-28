// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EInteractionResult : uint8
{
	IR_None			UMETA(DisplayName = "None"),
	IR_Started		UMETA(DisplayName = "Started"),
	IR_Successful	UMETA(DisplayName = "Successful"),
	IR_Failed		UMETA(DisplayName = "Failed"),
	IR_Interrupted	UMETA(DisplayName = "Interrupted")
};

UENUM(BlueprintType)
enum class EInteractionType : uint8
{
	IT_None		UMETA(DisplayName = "None"),
	IT_Instant	UMETA(DisplayName = "Instant"),
	IT_Hold		UMETA(DisplayName = "Hold")
};

UENUM(BlueprintType)
enum class EInteractionNetMode : uint8
{
	INM_None		UMETA(DisplayName="None"),
	INM_OwnerOnly	UMETA(DisplayName = "OwnerOnly"),
	INM_All			UMETA(DisplayName = "All")
};

