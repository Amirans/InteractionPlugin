// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractorComponent.h"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"
#include "UnrealNetwork.h"
#include "InteractionComponents/InteractionComponent.h"
#include "InteractionComponents/InteractionComponent_Hold.h"


UInteractorComponent::UInteractorComponent()
	:bInteracting(false)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UInteractorComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInteractorComponent, bInteracting);
}

// Called when the game starts
void UInteractorComponent::BeginPlay()
{
	Super::BeginPlay();

	/* Only Tick On Server and Owning Client */
	SetComponentTickEnabled(
		GetInteractorRemoteRole() == ROLE_Authority || GetInteractorRole() == ROLE_Authority
	);	
}

void UInteractorComponent::TryStartInteraction()
{
	/* Make Sure Interaction Starts on Authority */
	if (GetInteractorRole() != ROLE_Authority)
	{
		Server_TryStartInteraction();
		return;
	}

	/* Prevent New Interaction If One Already In Progress */
	if (bInteracting)
	{
		UE_LOG(LogInteractor, Warning, TEXT("Unable to Start Interaction Due to In Progress Interaction"));
		return;
	}

	/* Get Server Sided Interaction */
	if (!TryGetInteraction(InteractionCandidate))
	{
		UE_LOG(LogInteractor, Warning, TEXT("Unable to Start Interaction Due to No Interaction Available On Server Side"));
		return;
	}

	/* Start the Interaction */
	if (InteractionCandidate->CanInteractWith(this))
	{
		StartInteraction();
	}
}

void UInteractorComponent::Server_TryStartInteraction_Implementation()
{
	TryStartInteraction();
}

void UInteractorComponent::StartInteraction()
{

	SetInteracting(true);

	const bool bStarted = InteractionCandidate->StartInteraction(this);
	
	/* If Failed to Start , Fail Initiation */
	if (!bStarted)
	{
		SetInteracting(false);
		return;
	}

	/* Start Interactor Timer If Interaction Type is Hold */
	if (InteractionCandidate->GetInteractionType() == EInteractionType::IT_Hold)
	{
		UInteractionComponent_Hold* InteractionHold = Cast<UInteractionComponent_Hold>(InteractionCandidate);

		ToggleInteractorTimer(true,IsValid(InteractionHold) ? InteractionHold->GetInteractionDuration() : 0.1f);
	}

}

void UInteractorComponent::TryStopInteraction()
{
	if (GetInteractorRole() != ROLE_Authority)
	{
		Server_TryStopInteraction();
		return;
	}

	/* Return/Exit If No Interaction in Progress */
	if (!bInteracting  || !IsValid(InteractionCandidate))
	{
		return;
	}

	/* Cancel Interaction */
	InteractionCandidate->StopInteraction(this);
}

void UInteractorComponent::Server_TryStopInteraction_Implementation()
{
	TryStopInteraction();
}

void UInteractorComponent::EndInteraction(EInteractionResult InteractionResult, UInteractionComponent* InteractionComponent)
{

	/* Validate Ending Interaction Is Interactor Target Else Fail*/
	if (InteractionComponent != InteractionCandidate)
	{
		UE_LOG(LogInteractor, Warning, TEXT("Unable to Complete/End Interaction Due to Not Matching Interaction Targets"));

		InteractionResult = EInteractionResult::IR_Failed;
	}

	/* Set Interacting Status */
	SetInteracting(false);

	UE_LOG(LogInteractor, Log, TEXT("Ending Interaction"));

	/* Handle Notification Based On Interaction Result */
	switch (InteractionResult)
	{
	case EInteractionResult::IR_None:
		break;
	case EInteractionResult::IR_Successful:
		UE_LOG(LogTemp, Warning, TEXT("Interaction Successful"));
		break;
	case EInteractionResult::IR_Failed:
		UE_LOG(LogTemp, Warning, TEXT("Interaction Failed"));
		break;
	case EInteractionResult::IR_Interrupted:
		ToggleInteractorTimer(false);
		UE_LOG(LogTemp, Warning, TEXT("Interaction Interrupted"));
		break;
	default:
		break;
	}
}

void UInteractorComponent::OnInteractorTimerCompleted()
{
	UE_LOG(LogInteractor, Log, TEXT("Interactor Timer Completed"));

	UInteractionComponent_Hold* InteractionHold = Cast<UInteractionComponent_Hold>(InteractionCandidate);

	/* Validate Interaction Hold Component Is Valid */
	if (!IsValid(InteractionHold))
	{
		/* Fail Interaction to Prevent Deadlock */
		EndInteraction(EInteractionResult::IR_Failed, nullptr);

		UE_LOG(LogInteractor, Error, TEXT("Failed to Cast to Interaction Hold On %s"), *InteractionCandidate->GetName());
		return;
	}

	InteractionHold->OnHoldCompleted(this);
}

void UInteractorComponent::OnRep_bInteracting()
{

}

void UInteractorComponent::RegisterNewInteraction(UInteractionComponent* NewInteraction)
{

	if (GetInteractorRole() != ROLE_Authority)
	{
		/* Notify Interaction Of In Focus Status*/
		if (IsValid(InteractionCandidate))
		{
			InteractionCandidate->SetInteractionFocusState(true);
		}
	}

	InteractionCandidate = NewInteraction;

}

void UInteractorComponent::DeRegisterInteraction()
{

	if (GetInteractorRole() == ROLE_Authority)
	{
		//TODO: Cancel Interaction If Any in Progress
	}
	else
	{
		/* Notify Interaction Of Focus Lost  Status */
		if (IsValid(InteractionCandidate))
		{
			InteractionCandidate->SetInteractionFocusState(false);
		}
	}

	InteractionCandidate = nullptr;
}

// Called every frame
void UInteractorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

