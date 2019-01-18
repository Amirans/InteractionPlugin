// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractorComponentBase.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogInteractor, Log, All);

class UInteractionComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Abstract)
class INTERACTIONPLUGIN_API UInteractorComponentBase : public UActorComponent
{
	GENERATED_BODY()

public:	

	// Sets default values for this component's properties
	UInteractorComponentBase();


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
		return IsValid(GetOwner()) ? GetOwner()->GetLocalRole() : ENetRole::ROLE_None;
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

	


		
};
