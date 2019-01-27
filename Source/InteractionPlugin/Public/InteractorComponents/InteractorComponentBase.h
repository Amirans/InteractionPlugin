// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionDataTypes.h"
#include "InteractorComponentBase.generated.h"


DECLARE_LOG_CATEGORY_EXTERN(LogInteractor, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnInteractionUpdated, EInteractionResult, InteractionResult, EInteractionType, InteractionType, AActor*, InteractionActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNewInteraction, UInteractionComponent*, NewInteraction);

class UInteractionComponent;

UCLASS( ClassGroup=(Custom), Abstract)
class INTERACTIONPLUGIN_API UInteractorComponentBase : public UActorComponent
{
	GENERATED_BODY()

public:	

	// Sets default values for this component's properties
	UInteractorComponentBase();


	/**
	 * Delegate to Notify Interaction State Changes
	 */
	UPROPERTY(BlueprintAssignable)
		FOnInteractionUpdated OnInteractionUpdated;

	/**
	 * Delegate to Notify New Interaction Component in Focus
	 */
	UPROPERTY(BlueprintAssignable)
		FOnNewInteraction OnNewInteraction;


	/**
	 * [Config] Configuration to Determine Interaction State Over Net
	 *
	 * None		 : No Clients Receive the Interaction State Update
	 * OwnerOnly : Only the Local Owner of the Interactor Component Will Receive the Update
	 * All		 : All Clients With this Instance of the Interactor Component Will Receive the Update
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Interactor|Config", meta = (DisplayName = "State Net Mode"))
		EInteractionNetMode InteractorStateNetMode;

	/**
	 * Traces From the Eye Point of View
	 *
	 * @returns Returns the Interactable Components
	 */
	UFUNCTION()
		UInteractionComponent* GetInteractionTrace();

	/**
	 * Validates the Direction Towards Interaction Component
	 */
	UFUNCTION()
		bool ValidateDirection(const UInteractionComponent* InteractionComponent) const;

	/**
	 * Returns the Component Owner Role
	 */
	UFUNCTION(BlueprintPure, Category = Interactor)
		inline ENetRole GetInteractorRole() const
	{
		return IsValid(GetOwner()) ? GetOwner()->GetLocalRole() : ENetRole::ROLE_None;
	};

	/**
	 * Returns the Component Owner Remote Role
	 */
	UFUNCTION(BlueprintPure, Category = Interactor)
		inline ENetRole GetInteractorRemoteRole() const
	{
		return IsValid(GetOwner()) ? GetOwner()->GetRemoteRole() : ENetRole::ROLE_None;
	};

	/**
	 * Returns True If the Current Invoking is the Local Interactor
	 */
	UFUNCTION(BlueprintPure, Category = Interactor)
		inline bool IsLocalInteractor() const
	{
		if (GetNetMode() == NM_Standalone)
			return true;

		AController* InteractorController = GetOwner()->GetInstigatorController();

		return IsValid(InteractorController) && InteractorController->IsLocalController();
	};

	/**
	 * Performs an Interaction Trace and an Interaction Direction Validation
	 *
	 * @param OutInteractionComponent - Out Interaction Component If Found
	 * @returns True If Interaction Exists and has Valid Direction
	 */
	UFUNCTION()
		FORCEINLINE bool TryGetInteraction(UInteractionComponent*& OutInteractionComponent)
	{
		OutInteractionComponent = GetInteractionTrace();
		return ValidateDirection(OutInteractionComponent);
	}

protected:


	/**
	* Currently Interacting Interaction Component
	*/
	UPROPERTY(BlueprintReadOnly)
		UInteractionComponent* InteractionCandidate;

	/**
	 * Starts The Interactor Timer for a Given Duration
	 * 
	 * @param NewInteractionDuration - Duration to Start the Interactor Duration With
	 */
	UFUNCTION()
		void ToggleInteractorTimer(bool bStartTImer = true, float NewInteractionDuration = 0.1f);

	/**
	 * Function Invoked By Interactor Timer On Timer Ends
	 */
	UFUNCTION()
		virtual void OnInteractorTimerCompleted();

	/**
	 * Handles Interaction Notification Based on Config
	 *
	 * @param InteractionResult - Result of the Interaction
	 * @param InteractionType - Type of Interaction
	 */
	UFUNCTION()
		void NotifyInteraction(EInteractionResult InteractionResult, EInteractionType InteractionType);

	/**
	 * Owner Only Interaction State Notification
	 * 
	 * @param InteractionResult - Result of the Interaction
	 * @param InteractionType - Type of Interaction
	 */
	UFUNCTION(Client, Reliable)
		void Client_NotifyInteraction(EInteractionResult InteractionResult, EInteractionType InteractionType);

	/**
	 * Multi Cast Call to all Clients Notifying Interaction State
	 *
	 * @param InteractionResult - Result of the Interaction
	 * @param InteractionType - Type of Interaction
	 */
	UFUNCTION(NetMulticast, Reliable)
		void Multi_NotifyInteraction(EInteractionResult InteractionResult, EInteractionType InteractionType);

private:

	/**
	 * Timer to Handle Interaction Times
	 */
	UPROPERTY()
		FTimerHandle InteractorTimer;


	


		
};
