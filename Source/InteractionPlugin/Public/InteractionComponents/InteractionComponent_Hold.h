// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractionComponents/InteractionComponent.h"
#include "InteractionComponent_Hold.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class INTERACTIONPLUGIN_API UInteractionComponent_Hold : public UInteractionComponent
{
	GENERATED_BODY()

public:	

	// Sets default values for this component's properties
	UInteractionComponent_Hold();

	/**
	 * [Overridden] Starts Interaction for a Given Interactor Component
	 *
	 * @param InteractorComp - Interactor Component Trying to Interact
	 * @returns True Whether Interaction Initiated Successfully
	 */
	bool StartInteraction(UInteractorComponent* InteractorComp) override;

	/**
	 * Tries to Complete the Hold Interaction for the Interactor Component
	 * Invoked By Interactor on Timer Completion
	 * 
	 * @param InteractorComp - Interactor Component Trying to Complete the Hold
	 */
	void OnHoldCompleted(UInteractorComponent* InteractorComp);

	/**
	 * [Overridden] Stops an Interaction In Progress for a Given Interactor Component
	 *
	 * @param InteractorComp - Interactor Component Trying to Interact
	 * @returns True Whether Interaction Stopped Successfully
	 */
	bool StopInteraction(UInteractorComponent* InteractorComp) override;
	
	/**
	 * Getter for the Interaction Duration
	 */
	UFUNCTION(BlueprintGetter, Category = InteractionConfig)
		float GetInteractionDuration() const
	{
		return InteractionDuration;
	};

	/**
	 * Setter for the Interaction Duration
	 */
	UFUNCTION(BlueprintSetter, Category = InteractionConfig)
		void SetInteractionDuration(const float NewInteractionDuration)
	{
		InteractionDuration = NewInteractionDuration;
	};

protected:

	/** Duration Required for the Interaction to be Completed */
	UPROPERTY(EditDefaultsOnly, Category = InteractionConfig, BlueprintGetter = GetInteractionDuration, BlueprintSetter = SetInteractionDuration,meta = (ClampMin = 0.0f))
		float InteractionDuration;

private:

	/**
	 * Calculates and Returns If the Initiated Interaction Time Plus the Interaction Required Duration has already been passed.
	 *
	 * @param InteractorComponent - Interactor to Check Against
	 * @returns True If Duration is Over
	 */
	UFUNCTION()
		inline bool IsInteractionTimeOver(const UInteractorComponent* InteractorComponent) const;

	/**
	 * Adds Interactor and the Current Game World Time to the Interactor Map 
	 */
	UFUNCTION()
		void AddInteractor(UInteractorComponent* InteractorComponent);

	/**
	 * Map of Interactors to Interaction Initiation Game Time
	 */
	UPROPERTY()
		TMap<UInteractorComponent*, float> Interactors;
		
};
