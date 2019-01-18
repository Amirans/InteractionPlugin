// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractorComponent.h"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"
#include "UnrealNetwork.h"
#include "InteractionComponents/InteractionComponent.h"


// Sets default values for this component's properties
UInteractorComponent::UInteractorComponent()
	:bInteracting(false)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	this->SetIsReplicated(true);

	// ...
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

	if (TryGetInteraction(InteractionCandidate))
	{
		/* Start Interacting */
		SetInteracting(true);
		
		InteractionCandidate->StartInteraction(this);

		UE_LOG(LogInteractor, Log, TEXT("Interaction Initiation Requested"));
	}
}

void UInteractorComponent::Server_TryStartInteraction_Implementation()
{
	TryStartInteraction();
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
		break;
	default:
		break;
	}
}

void UInteractorComponent::OnRep_bInteracting()
{

}

void UInteractorComponent::RegisterNewInteraction(UInteractionComponent* NewInteraction)
{

	if (GetInteractorRole() == ROLE_Authority)
	{

	}
	else
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

