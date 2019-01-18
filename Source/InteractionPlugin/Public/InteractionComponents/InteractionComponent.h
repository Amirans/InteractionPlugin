// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "InteractionComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionFocusChanged, bool, bInFocus);


DECLARE_LOG_CATEGORY_EXTERN(LogInteraction, Log, All);

class UInteractorComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Abstract)
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

protected:

	// Called when the game starts
	virtual void BeginPlay() override;

	/**
	 * Completes an Interaction With an Interactor
	 *
	 * Must be the End Point For All the Interaction Completions
	 */
	UFUNCTION()
		virtual void CompleteInteraction(bool bSuccessful, UInteractorComponent* InteractorComp);	
};
