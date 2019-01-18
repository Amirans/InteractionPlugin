// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractionComponents/InteractionComponent.h"
#include "InteractionComponent_Instant.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class INTERACTIONPLUGIN_API UInteractionComponent_Instant : public UInteractionComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInteractionComponent_Instant();

	/**
	 * [Overridden] Starts Interaction for a Given Interactor Component 
	 *
	 * @param InteractorComp - Interactor Component Trying to Interact
	 * @returns True Whether Interaction Initiated Successfully
	 */
	bool StartInteraction(UInteractorComponent* InteractorComp) override;

};
