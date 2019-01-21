// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "InteractionDataTypes.h"
#include "InteractionComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionFocusChanged, bool, bInFocus);


DECLARE_LOG_CATEGORY_EXTERN(LogInteraction, Log, All);

class UInteractorComponent;

UCLASS( ClassGroup=(Custom), Abstract)
class INTERACTIONPLUGIN_API UInteractionComponent : public USceneComponent
{
	GENERATED_BODY()

public:	

	// Sets default values for this component's properties
	UInteractionComponent();

	/* Delegate to Notify Interactor Focus Changed */
	UPROPERTY(BlueprintAssignable)
		FOnInteractionFocusChanged OnInteractionFocusChanged;

	/**
	 * Starts Interaction for a Given Interactor Component
	 *
	 * @param InteractorComp - Interactor Component Trying to Interact
	 * @returns True Whether Interaction Initiated Successfully
	 */
	UFUNCTION(BlueprintCallable, Category = InteractionComponent)
		virtual bool StartInteraction(UInteractorComponent* InteractorComp);

	/**
	 * Stops an Interaction In Progress for a Given Interactor Component
	 * 
	 * @param InteractorComp - Interactor Component Trying to Interact
	 * @returns True Whether Interaction Stopped Successfully
	 */
	UFUNCTION(BlueprintCallable, Category = InteractionComponent)
		virtual bool StopInteraction(UInteractorComponent* InteractorComp);
	

	/**
	 * Validates Condition and Returns Whether Interaction is Allowed
	 */
	UFUNCTION(BlueprintCallable, Category = InteractionComponent)
		virtual bool CanInteractWith(UInteractorComponent* InteractoComp);

	/**
	 * Invoked By Interactor to Notify Interaction is Under Focus
	 */
	UFUNCTION()
		void SetInteractionFocusState(bool bNewFocus);

	/**
	 * Getter For Interaction Component Interaction Type
	 */
	UFUNCTION(BlueprintGetter, Category = InteractionComponent)
		EInteractionType GetInteractionType() const
	{
		return InteractionType;
	}

protected:

	// Called when the game starts
	virtual void BeginPlay() override;

	/**
	 * Interaction Component Interaction Type
	 */
	UPROPERTY(BlueprintGetter = GetInteractionType, Category = InteractionComponent)
		EInteractionType InteractionType;

	/**
	 * Completes an Interaction With an Interactor
	 *
	 * @note Must be the End Point For All the Interaction Completions
	 * @param InteractionResult - Result of Interaction
	 * @param InteractorComp - Interactor Component to Notify
	 */
	UFUNCTION()
		virtual void CompleteInteraction(EInteractionResult InteractionResult, UInteractorComponent* InteractorComp);
};
