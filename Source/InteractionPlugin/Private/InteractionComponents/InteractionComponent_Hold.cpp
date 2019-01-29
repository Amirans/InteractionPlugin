// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractionComponent_Hold.h"
#include "Engine/World.h"
#include "InteractorComponents/InteractorComponent.h"

UInteractionComponent_Hold::UInteractionComponent_Hold()
	:InteractionDuration(10.0f)
{
	PrimaryComponentTick.bCanEverTick = false;
	InteractionType = EInteractionType::IT_Hold;
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
		/* Complete the Interaction */
		CompleteInteraction(EInteractionResult::IR_Successful, InteractorComp);
	}
	else
	{
		/* Fail Interaction If Time Not Over */
		CompleteInteraction(EInteractionResult::IR_Failed, InteractorComp);
	}

	/* Remove the Interactor from List */
	Interactors.Remove(InteractorComp);
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
		CompleteInteraction(EInteractionResult::IR_Interrupted, InteractorComp);
	}

	/* Remove the Interactor from List */
	Interactors.Remove(InteractorComp);

	return true;
}

bool UInteractionComponent_Hold::CanInteractWith(UInteractorComponent* InteractoComp)
{
	return Super::CanInteractWith(InteractoComp) && bMultipleInteraction || Interactors.Num() == 0;
}

bool UInteractionComponent_Hold::IsInteractionTimeOver(const UInteractorComponent* InteractorComponent) const
{
	if (Interactors.Contains(InteractorComponent))
	{
		const UWorld* World = GetWorld();

		/* Interaction Duration With Addition Error Tolerance of 0.5 */
		const float ErrorToleranceDuration = InteractionDuration - 0.5f;

		return IsValid(World) ? Interactors[InteractorComponent] + ErrorToleranceDuration <= World->GetTimeSeconds() : false;

	}

	return false;
}

void UInteractionComponent_Hold::AddInteractor(UInteractorComponent* InteractorComponent)
{
	/* Get World and Time Seconds */
	const UWorld* World = GetWorld();
	
	Interactors.Add(InteractorComponent, IsValid(World) ? World->GetTimeSeconds() : 0.0f);
}
