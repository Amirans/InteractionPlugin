// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractionComponent_Hold.h"
#include "Engine/World.h"
#include "InteractorComponents/InteractorComponent.h"

UInteractionComponent_Hold::UInteractionComponent_Hold()
	:InteractionDuration(10.0f)
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UInteractionComponent_Hold::StartInteraction(UInteractorComponent* InteractorComp)
{
	/* Exit If Super Returns False */
	if (!Super::StartInteraction(InteractorComp))
	{
		return false;
	}

	/* Record Initiation Time */
	AddInteractor(InteractorComp);

	return true;
}

void UInteractionComponent_Hold::OnHoldCompleted(UInteractorComponent* InteractorComp)
{
	if (IsInteractionTimeOver(InteractorComp))
	{
		/* Remove the Interactor from List */
		Interactors.Remove(InteractorComp);

		/* Complete the Interaction */
		CompleteInteraction(true, InteractorComp);
	}
}

bool UInteractionComponent_Hold::StopInteraction(UInteractorComponent* InteractorComp)
{
	/* Exit If Super Returns False */
	if (!Super::StopInteraction(InteractorComp))
	{
		return false;
	}

	/* Notify Interactor of Interaction Interruption */
	if (Interactors.Contains(InteractorComp))
	{
		InteractorComp->EndInteraction(EInteractionResult::IR_Interrupted, this);
	}

	/* Remove the Interactor from List */
	Interactors.Remove(InteractorComp);

	return true;
}

bool UInteractionComponent_Hold::IsInteractionTimeOver(const UInteractorComponent* InteractorComponent) const
{
	if (Interactors.Contains(InteractorComponent))
	{
		const UWorld* World = GetWorld();

		/* Return True If the Interaction Initiated Time Plus the Duration Has Been Passed*/
		return IsValid(World) ? Interactors[InteractorComponent] + InteractionDuration <= World->GetTimeSeconds() : false;
	}

	return false;
}

void UInteractionComponent_Hold::AddInteractor(UInteractorComponent* InteractorComponent)
{
	/* Get World and Time Seconds */
	const UWorld* World = GetWorld();
	
	Interactors.Add(InteractorComponent, IsValid(World) ? World->GetTimeSeconds() : 0.0f);
}
