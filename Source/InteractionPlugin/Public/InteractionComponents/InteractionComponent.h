// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "InteractionDataTypes.h"
#include "InteractionComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInteractionStateChanged, EInteractionResult, InteractionResult, AActor*, InteractionActor);
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
	 * Delegate to Notify Interaction State Changes
	 */
	UPROPERTY(BlueprintAssignable)
		FOnInteractionStateChanged OnInteractionStateChanged;

	/**
	 * [Config] Configuration to Determine Interaction State Over Net
	 *
	 * None		 : No Clients Receive the Interaction State Update
	 * OwnerOnly : Only the Local Owner of the Interaction Component Will Receive the Update
	 * All		 : All Clients With this Instance of the Interaction Component Will Receive the Update
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "InteractionComponent|Config", meta = (DisplayName = "State Net Mode"))
		EInteractionNetMode InteractionStateNetMode;

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
		virtual bool CanInteractWith(UInteractorComponent* InteractorComp);

	/**
	 * Invoked By Interactor to Notify Interaction is Under Focus
	 */
	UFUNCTION()
		void SetInteractionFocusState(bool bNewFocus);

	/**
	 * Owner Only Interaction State Notification
	 *
	 * @note Owner Only Interaction State is Handled and Recieved by Interactor
	 * @param InteractionResult - Result of the Interaction
	 * @param InteractionType - Type of Interaction
	 */
	UFUNCTION()
		void ClientNotifyInteraction(EInteractionResult NewInteractionResult, UInteractorComponent* NewInteractionComponent);

	/**
	 * Getter For Interaction Component Interaction Type
	 */
	UFUNCTION(BlueprintGetter, Category = InteractionComponent)
		EInteractionType GetInteractionType() const
	{
		return InteractionType;
	}

protected:

	/**
	 * Interaction Component Interaction Type
	 */
	UPROPERTY(BlueprintGetter = GetInteractionType, Category = InteractionComponent)
		EInteractionType InteractionType;

	/**
	 * [Config] Boolean to Allow Multiple Interaction at Once
	 * @note Does Not Apply to Instant Interaction
	 */
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "InteractionComponent|Config", meta=(DisplayName="Allow Multiple Interaction"))
		bool bMultipleInteraction;

	/**
	 * Completes an Interaction With an Interactor
	 *
	 * @note Must be the End Point For All the Interaction Completions
	 * @param InteractionResult - Result of Interaction
	 * @param InteractorComp - Interactor Component to Notify
	 */
	UFUNCTION()
		virtual void CompleteInteraction(EInteractionResult InteractionResult, UInteractorComponent* InteractorComp);

	/**
	 * Handles Interaction Notification Based on Config
	 *
	 * @param InteractionResult - Result of the Interaction
	 * @param InteractionType - Type of Interaction
	 */
	UFUNCTION()
		void NotifyInteraction(EInteractionResult NewInteractionResult, UInteractorComponent* NewInteractionComponent);

	/**
	 * Multi Cast Call to all Clients Notifying Interaction State
	 *
	 * @param InteractionResult - Result of the Interaction
	 * @param InteractionType - Type of Interaction
	 */
	UFUNCTION(NetMulticast, Reliable)
		void Multi_NotifyInteraction(EInteractionResult NewInteractionResult, UInteractorComponent* NewInteractionComponent);
};
