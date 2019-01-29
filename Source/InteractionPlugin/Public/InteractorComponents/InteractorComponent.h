// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionDataTypes.h"
#include "InteractorComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogInteractor, Log, All);

class UInteractionComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnInteractorStateChanged, EInteractionResult, InteractionResult, EInteractionType, InteractionType, AActor*, InteractionActor);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNewInteraction, UInteractionComponent*, NewInteraction);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class INTERACTIONPLUGIN_API UInteractorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	// Sets default values for this component's properties
	UInteractorComponent();

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

	/**
	 * Delegate to Notify Interaction State Changes
	 */
	UPROPERTY(BlueprintAssignable)
		FOnInteractorStateChanged OnInteractorStateChanged;

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
	 * [Config] Define the Distance Which an Interaction Component Will be Detected and be In Focus
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Interactor|Config")
		float InteractorReachLength;


#if WITH_EDITORONLY_DATA

	/**
	 * [Editor Only] Debug Boolean to Determine Whether Trace Line Should be Drawn
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Interactor|Debug")
		bool bDrawInteractorTrace;

#endif

	/**
	 * Tries to Start Interaction On Authority Side of the Interactor
	 * Gets Interaction On Server Side to Start
	 */
	UFUNCTION(BlueprintCallable, Category = Interactor)
		void TryStartInteraction();

	/**
	 * Tries to Stop an Interaction on Authority Side of the Interaction
	 */
	UFUNCTION(BlueprintCallable, Category = Interactor)
		void TryStopInteraction();

	/**
	 * Ends the Interaction with Result
	 *
	 * @param InteractionResult - Result of the Interaction Process
	 */
	UFUNCTION()
		void EndInteraction(EInteractionResult InteractionResult,UInteractionComponent* InteractionComponent);

	/**
	 * Validates Condition and Returns Whether Interaction is Allowed 
	 */
	UFUNCTION(BlueprintCallable, Category = Interactor)
		bool CanInteractWith(UInteractionComponent* InteractionComponent);

	/**
	* Returns the Component Owner Role
	*/
	UFUNCTION(BlueprintPure, Category = Interactor)
		FORCEINLINE ENetRole GetInteractorRole() const
	{
		return IsValid(GetOwner()) ? GetOwner()->GetLocalRole() : ENetRole::ROLE_None;
	};

	/**
	* Returns the Component Owner Remote Role
	*/
	UFUNCTION(BlueprintPure, Category = Interactor)
		FORCEINLINE ENetRole GetInteractorRemoteRole() const
	{
		return IsValid(GetOwner()) ? GetOwner()->GetRemoteRole() : ENetRole::ROLE_None;
	};


	/**
	 * Returns True If the Current Invoking is the Local Interactor
	 */
	UFUNCTION(BlueprintPure, Category = Interactor)
		FORCEINLINE bool IsLocalInteractor() const
	{
		const ENetMode NetMode = GetNetMode();
		const ENetRole Role = GetInteractorRole();

		if (NetMode == NM_Standalone)
		{
			return true;
		}

		if (NetMode == NM_Client && Role == ROLE_AutonomousProxy)
		{
			return true;
		}

		if (GetInteractorRemoteRole() != ROLE_AutonomousProxy && Role == ROLE_Authority)
		{
			return true;
		}

		return false;
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
	 * Boolean to Determine Whether the Interactor is Interacting or Not
	 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_bInteracting)
		bool bInteracting;

	/**
	 * Currently Interacting Interaction Component
	 */
	UPROPERTY(BlueprintReadOnly)
		UInteractionComponent* InteractionCandidate;

	// Called when the game starts
	virtual void BeginPlay() override;

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

protected:


	/**
	 * Starts The Interaction With the Interaction Component
	 * @note Should Not Be Called Directly, @see TryStartInteraction
	 */
	UFUNCTION()
		void StartInteraction();

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
		void OnInteractorTimerCompleted();

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
	 * Timer to Handle Interaction Times
	 */
	UPROPERTY()
		FTimerHandle InteractorTimer;

	/**
	 * RPC to Server To Start the Interaction
	 */
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_TryStartInteraction();
		bool Server_TryStartInteraction_Validate() { return true; };

	/**
	* RPC to Server To Stop the Interaction
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_TryStopInteraction();
		bool Server_TryStopInteraction_Validate() { return true; };
	
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

	/**
	 * Determines Whether this Interactor Instance Should Tick
	 *
	 * @return True If Local is Owner Or Instance is on Server
	 */
	UFUNCTION()
		FORCEINLINE bool ShouldTickInstance() const
	{
		return GetNetMode() == NM_Client && GetInteractorRole() != ROLE_AutonomousProxy ? false : true;
	}

	/**
	 * Handles Interaction Notification Based on Config
	 *
	 * @param InteractionResult - Result of the Interaction
	 * @param InteractionType - Type of Interaction
	 */
	UFUNCTION()
		void NotifyInteraction(EInteractionResult NewInteractionResult, EInteractionType NewInteractionType);

	/**
	 * Owner Only Interaction State Notification
	 *
	 * @param InteractionResult - Result of the Interaction
	 * @param InteractionType - Type of Interaction
	 */
	UFUNCTION(Client, Reliable)
		void Client_NotifyInteraction(EInteractionResult NewInteractionResult, EInteractionType NewInteractionType);

	/**
	 * Multi Cast Call to all Clients Notifying Interaction State
	 *
	 * @param InteractionResult - Result of the Interaction
	 * @param InteractionType - Type of Interaction
	 */
	UFUNCTION(NetMulticast, Reliable)
		void Multi_NotifyInteraction(EInteractionResult NewInteractionResult, EInteractionType NewInteractionType);




public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;		
};
