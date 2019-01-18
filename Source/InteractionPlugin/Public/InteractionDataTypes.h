// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EInteractionResult : uint8
{
	IR_None,
	IR_Successful,
	IR_Failed,
	IR_Interrupted
};