// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractorComponentBase.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "InteractionComponents/InteractionComponent.h"
#include "DrawDebugHelpers.h"

DEFINE_LOG_CATEGORY(LogInteractor);

UInteractorComponentBase::UInteractorComponentBase()
{
	PrimaryComponentTick.bCanEverTick = true;
	this->SetIsReplicated(true);
}

UInteractionComponent* UInteractorComponentBase::GetInteractionTrace()
{
	/* Get World */
	const UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return nullptr;
	}

	/* Get Owner */
	const AActor* Owner = GetOwner();
	if (!IsValid(Owner))
	{
		return nullptr;
	}

	/* Set Query Params */
	const FName TraceTag("InteractionTrace");
	FCollisionQueryParams QueryParams = FCollisionQueryParams(TraceTag, true, Owner);

	/* Get Start Location and Rotation */
	FVector OutLocation;
	FRotator OutRotator;

	Owner->GetActorEyesViewPoint(OutLocation, OutRotator);

	const FVector StartLocation = OutLocation;
	const FVector EndLocation = (OutRotator.Vector() * 1200.0f) + OutLocation;

	/* Prepare Hit */
	FHitResult OutHit;

	DrawDebugLine(World, StartLocation, EndLocation, FColor::Red, false, 0.0f, 1.f);

	/* Single Line Trace */
	const bool bHit = World->LineTraceSingleByChannel(OutHit, StartLocation, EndLocation, ECollisionChannel::ECC_Visibility, QueryParams);

	/* Get Interaction Component */
	if (bHit)
	{
		UActorComponent* ActorComp = OutHit.Actor->GetComponentByClass(UInteractionComponent::StaticClass());

		return Cast <UInteractionComponent>(ActorComp);
	}

	return nullptr;

}

bool UInteractorComponentBase::ValidateDirection(const UInteractionComponent* InteractionComponent) const
{
	if (!IsValid(InteractionComponent))
	{
		return false;
	}

	/* Get Interactor Direction from Our Location */
	FVector Direction = GetOwner()->GetActorLocation() - InteractionComponent->GetComponentLocation();
	Direction.Normalize();

	float DirectionPoint = 0.0f;

	return FVector::DotProduct(Direction, InteractionComponent->GetForwardVector()) > 0.5f;
}

void UInteractorComponentBase::ToggleInteractorTimer(bool bStartTImer /*= true*/, float NewInteractionDuration /*= 0.1f*/)
{
	/* Get World */
	const UWorld* World = GetWorld();

	if (!IsValid(World))
	{
		UE_LOG(LogInteractor, Warning, TEXT("Unable to Toggle Hold Interaction Due to Null World"));
		return;
	}

	if (bStartTImer)
	{
		/* Start The Timer */
		World->GetTimerManager().SetTimer(InteractorTimer, this, &UInteractorComponentBase::OnInteractorTimerCompleted, NewInteractionDuration);
	}
	else
	{
		/* Clear The Timer */
		World->GetTimerManager().ClearTimer(InteractorTimer);
	}
	
}

void UInteractorComponentBase::OnInteractorTimerCompleted()
{
	//Invoked By Interactor Timer On Timer Completed 
}

void UInteractorComponentBase::NotifyInteraction(EInteractionResult InteractionResult, EInteractionType InteractionType)
{
	//TODO: Implement Config and Multicast Call
	Client_NotifyInteraction(InteractionResult, InteractionType);
}

void UInteractorComponentBase::Client_NotifyInteraction_Implementation(EInteractionResult InteractionResult, EInteractionType InteractionType)
{
	if (OnInteractionUpdated.IsBound())
	{
		OnInteractionUpdated.Broadcast(
			InteractionResult, 
			InteractionType, 
			IsValid(InteractionCandidate) ? InteractionCandidate->GetOwner() : nullptr
		);
	}
}

void UInteractorComponentBase::Multi_NotifyInteraction_Implementation(EInteractionResult InteractionResult, EInteractionType InteractionType)
{
	if (OnInteractionUpdated.IsBound())
	{
		OnInteractionUpdated.Broadcast(
			InteractionResult,
			InteractionType,
			IsValid(InteractionCandidate) ? InteractionCandidate->GetOwner() : nullptr
		);
	}
}
