// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractorComponents/InteractorComponentBase.h"
#include "InteractorComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNewInteraction, const UInteractionComponent*, NewInteraction);

class UInteractionComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class INTERACTIONPLUGIN_API UInteractorComponent : public UInteractorComponentBase
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInteractorComponent();

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

	/**
	 * Tries to Start Interaction On Authority Side of the Interactor
	 * Gets Interaction On Server Side to Start
	 */
	UFUNCTION(BlueprintCallable, Category = Interactor)
		void TryStartInteraction();

	/**
	 * Tries to Cancel an Interaction on Authority Side of the INteraction
	 */
	UFUNCTION(BlueprintCallable, Category = Interactor)
		void TryCancelInteraction();

	/**
	* Ends the Interaction with Result
	*
	* @param InteractionResult - Result of the Interaction Process
	*/
	UFUNCTION()
		void EndInteraction(EInteractionResult InteractionResult,UInteractionComponent* InteractionComponent);

protected:

	// Called when the game starts
	virtual void BeginPlay() override;

	/**
	 * Boolean to Determine Whether the Interactor is Interacting or Not
	 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_bInteracting)
		bool bInteracting;

protected:


	/**
	 * Starts The Interaction With the Interaction Component
	 * @note Should Not Be Called Directly, @see TryStartInteraction
	 */
	UFUNCTION()
		void StartInteraction();

	/**
	 * Function Invoked By Interactor Timer On Timer Ends
	 */
		void OnInteractorTimerCompleted() override;

	/**
	 * On Rep To Notify of bInteracting Changes
	 */
	UFUNCTION()
		void OnRep_bInteracting();

	/**
	 * Sets bInteracting Value and Calls Related On Rep
	 */
	UFUNCTION()
		void SetInteracting(bool bNewInteracting) 
	{
		bInteracting = bNewInteracting;

		if (GetNetMode() != ENetMode::NM_DedicatedServer)
		{
			OnRep_bInteracting();
		}
	};

	

private:

	/**
	 * RPC to Server To Start the Interaction
	 */
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_TryStartInteraction();
		bool Server_TryStartInteraction_Validate() { return true; };

	/**
	* RPC to Server To Cancel the Interaction
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_TryCancelInteraction();
		bool Server_TryCancelInteraction_Validate() { return true; };
	
	/**
	 * Invoked When a New Interaction Component is Valid Candidate
	 *
	 * @param NewInteraction - New Interaction Component
	 */
	UFUNCTION()
		void RegisterNewInteraction(UInteractionComponent* NewInteraction);

	/**
	 * Invoked When Interaction is Not Valid for Candidate
	 *
	 * Removes the Current Interaction Candidate, Requests to Cancel any Interaction in Progress
	 */
	UFUNCTION()
		void DeRegisterInteraction();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


		
};
