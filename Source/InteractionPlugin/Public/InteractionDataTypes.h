// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EInteractionResult : uint8
{
	IR_None,
	IR_Started,
	IR_Successful,
	IR_Failed,
	IR_Interrupted
};

UENUM(BlueprintType)
enum class EInteractionType : uint8
{
	IT_None,
	IT_Instant,
	IT_Hold
};