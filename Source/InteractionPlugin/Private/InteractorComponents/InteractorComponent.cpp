// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractorComponent.h"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"
#include "UnrealNetwork.h"
#include "InteractionComponents/InteractionComponent.h"
#include "InteractionComponents/InteractionComponent_Hold.h"
#include "Interface/InteractionInterface.h"
#include "DrawDebugHelpers.h"

DEFINE_LOG_CATEGORY(LogInteractor);

UInteractorComponent::UInteractorComponent()
	:bInteracting(false),
	InteractorStateNetMode(EInteractionNetMode::INM_OwnerOnly)
{
	PrimaryComponentTick.bCanEverTick = true;
	this->SetIsReplicated(true);
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
		ShouldTickInstance()
	);	
}

UInteractionComponent* UInteractorComponent::GetInteractionTrace()
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

bool UInteractorComponent::ValidateDirection(const UInteractionComponent* InteractionComponent) const
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
	if (InteractionCandidate->CanInteractWith(this) &&
		CanInteractWith(InteractionCandidate))
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

		/* Notify Interaction Failed Result */
		NotifyInteraction(EInteractionResult::IR_Failed, InteractionCandidate->GetInteractionType());

		return;
	}

	/* Start Interactor Timer If Interaction Type is Hold */
	if (InteractionCandidate->GetInteractionType() == EInteractionType::IT_Hold)
	{
		UInteractionComponent_Hold* InteractionHold = Cast<UInteractionComponent_Hold>(InteractionCandidate);

		ToggleInteractorTimer(true,IsValid(InteractionHold) ? InteractionHold->GetInteractionDuration() : 0.1f);
	}

	/* Notify Interaction Started Result */
	NotifyInteraction(EInteractionResult::IR_Started, InteractionCandidate->GetInteractionType());
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

	/* Notify Interaction Started Result */
	NotifyInteraction(InteractionResult, InteractionCandidate->GetInteractionType());
}

bool UInteractorComponent::CanInteractWith(UInteractionComponent* InteractionComponent)
{
	/* Get Owner */
	AActor* Owner = GetOwner();

	/* Check for Interaction Interface and Execute If Owner Implements */
	if (IsValid(Owner) &&
		Owner->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
	{

		return IInteractionInterface::Execute_ICanInteractWith(Owner, InteractionCandidate->GetOwner());
	}

	return true;
}

void UInteractorComponent::ToggleInteractorTimer(bool bStartTImer /*= true*/, float NewInteractionDuration /*= 0.1f*/)
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
		World->GetTimerManager().SetTimer(InteractorTimer, this, &UInteractorComponent::OnInteractorTimerCompleted, NewInteractionDuration);
	}
	else
	{
		/* Clear The Timer */
		World->GetTimerManager().ClearTimer(InteractorTimer);
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
	if (!IsValid(NewInteraction))
	{
		UE_LOG(LogInteractor, Warning, TEXT("Unable to Register New Interaction Due to Invalid Interaction Component"));
		return;
	}

	/* Prevent Duplicate Registration */
	if (InteractionCandidate == NewInteraction)
	{
		return;
	}

	InteractionCandidate = NewInteraction;

	/* Local Interactor */
	if (IsLocalInteractor())
	{
		NewInteraction->SetInteractionFocusState(true);

		if (OnNewInteraction.IsBound())
		{
			OnNewInteraction.Broadcast(NewInteraction);
		}
	}
}

void UInteractorComponent::DeRegisterInteraction()
{
	/* Cancel Interaction If Already Interacting */
	if (bInteracting)
	{
		TryStopInteraction();
	}

	/* Local Interactor */
	if (IsLocalInteractor())
	{
		if (IsValid(InteractionCandidate))
		{
				InteractionCandidate->SetInteractionFocusState(false);
		}

		if (OnNewInteraction.IsBound())
		{
				OnNewInteraction.Broadcast(nullptr);
		}
	}

	InteractionCandidate = nullptr;
}

void UInteractorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bInteracting)
	{
		/* If Interacting Get the New Interaction Candidate and Compare to the Current Interacting Component*/
		const UInteractionComponent* NewInteraction = GetInteractionTrace();

		if (!ValidateDirection(NewInteraction) || NewInteraction != InteractionCandidate)
		{
			/* Cancel Interaction If not Valid Interaction */
			DeRegisterInteraction();
		}

	}
	else if(IsLocalInteractor())
	{
		/* Locally Get Interaction and Validate the Component */
		UInteractionComponent* NewInteraction = GetInteractionTrace();

		if (ValidateDirection(NewInteraction))
		{
			/* Register If New Interaction is Not Equal to the Current Candidate */
			if (NewInteraction != InteractionCandidate)
			{
				RegisterNewInteraction(NewInteraction);
			}
		}
		else if(IsValid(InteractionCandidate))
		{
			/* DeRegister Interaction If No Valid Interaction Component Exits*/
			DeRegisterInteraction();
		}
		
	}
}

void UInteractorComponent::NotifyInteraction(EInteractionResult NewInteractionResult, EInteractionType NewInteractionType)
{
	switch (InteractorStateNetMode)
	{
	case EInteractionNetMode::INM_OwnerOnly:
		Client_NotifyInteraction(NewInteractionResult, NewInteractionType);
		break;
	case EInteractionNetMode::INM_All:
		Multi_NotifyInteraction(NewInteractionResult, NewInteractionType);
		break;
	default:
		break;
	}
}

void UInteractorComponent::Client_NotifyInteraction_Implementation(EInteractionResult NewInteractionResult, EInteractionType NewInteractionType)
{
	if (OnInteractorStateChanged.IsBound())
	{
		OnInteractorStateChanged.Broadcast(
			NewInteractionResult,
			NewInteractionType,
			IsValid(InteractionCandidate) ? InteractionCandidate->GetOwner() : nullptr
		);
	}

	/* Notify Interaction Locally If Interaction Net Mode is Owner Only*/
	if (IsValid(InteractionCandidate) &&
		InteractionCandidate->InteractionStateNetMode == EInteractionNetMode::INM_OwnerOnly)
	{
		InteractionCandidate->ClientNotifyInteraction(NewInteractionResult, this);
	}
}

void UInteractorComponent::Multi_NotifyInteraction_Implementation(EInteractionResult NewInteractionResult, EInteractionType NewInteractionType)
{
	if (OnInteractorStateChanged.IsBound())
	{
		OnInteractorStateChanged.Broadcast(
			NewInteractionResult,
			NewInteractionType,
			IsValid(InteractionCandidate) ? InteractionCandidate->GetOwner() : nullptr
		);
	}
}
