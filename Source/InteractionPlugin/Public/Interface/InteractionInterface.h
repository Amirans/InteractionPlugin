// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractionInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UInteractionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class INTERACTIONPLUGIN_API IInteractionInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	/**
	 * Interface Invoked By Interactor and Interaction Component before an Interaction Process to Custom Conditions
	 *
	 * @param OtherOwner - Interaction or Interactor Owner Trying to Interact With
	 * @return True If Interaction is Allowed else Interaction will not Start
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = InteractionInterface)
		bool ICanInteractWith(AActor* OtherOwner);
};
