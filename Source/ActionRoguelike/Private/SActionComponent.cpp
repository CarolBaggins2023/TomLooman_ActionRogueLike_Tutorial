// Fill out your copyright notice in the Description page of Project Settings.


#include "SActionComponent.h"

#include "ActionRoguelike/ActionRoguelike.h"
#include "Engine/ActorChannel.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
USActionComponent::USActionComponent() {

	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
}


// Called when the game starts
void USActionComponent::BeginPlay() {
	Super::BeginPlay();

	if (GetOwner()->HasAuthority()) {
		for (TSubclassOf<USAction> ActionClass : DefaultActions) {
			AddAction(GetOwner(), ActionClass);
		}
	}
}

// Called every frame
void USActionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// FString DebugMsg = GetNameSafe(GetOwner()) + " : " + ActiveGameplayTags.ToStringSimple();
	// GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::White, DebugMsg);

	/*for (USAction *Action : Actions) {
		FColor TextColor = Action->IsRunning() ? FColor::Blue : FColor::White;

		FString ActionMsg = FString::Printf(TEXT("[%s] Action: %s"), *GetNameSafe(GetOwner()), *GetNameSafe(Action));

		LogOnScreen(this, ActionMsg, TextColor, 0.0f);
	}*/
}

void USActionComponent::AddAction(AActor *Instigator, TSubclassOf<USAction> ActionClass) {
	if (!ensure(ActionClass)) {
		return;
	}

	if (!GetOwner()->HasAuthority()) {
		UE_LOG(LogTemp, Warning, TEXT("Client attempting to AddAction. [Class: %s]"), *GetNameSafe(ActionClass));
		return;
	}

	// USAction is the type of the pointer. ActionClass is the type of the new project.
	USAction *NewAction = NewObject<USAction>(GetOwner(), ActionClass);
	if (ensure(NewAction)) {

		NewAction->Initialize(this);
		
		Actions.Add(NewAction);

		if (NewAction->bAutoStart && ensure(NewAction->CanStart(Instigator))) {
			NewAction->StartAction(Instigator);
		}
	}
}

void USActionComponent::RemoveAction(USAction *ActionToRemove) {
	if (!ensure(ActionToRemove && !ActionToRemove->IsRunning())) {
		return;
	}

	Actions.Remove(ActionToRemove);
}

bool USActionComponent::StartActionByName(AActor* Instigator, FName ActionName) {
	for (USAction *Action : Actions) {
		if (Action && Action->ActionName == ActionName) {
			if (!Action->CanStart(Instigator)) {
				FString FailedMsg = FString::Printf(TEXT("Failed to run: %s"), *ActionName.ToString());
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FailedMsg);
				continue;
			}

			// If we are on client.
			if (!GetOwner()->HasAuthority()) {
				ServerStartAction(Instigator, ActionName);
			}
			
			Action->StartAction(Instigator);
			return true;
		}
	}
	return false;
}

bool USActionComponent::StopActionByName(AActor* Instigator, FName ActionName) {
	for (USAction *Action : Actions) {
		if (Action && Action->ActionName == ActionName) {
			if (Action->IsRunning()) {

				// If we are on client.
				if (!GetOwner()->HasAuthority()) {
					ServerStopAction(Instigator, ActionName);
				}
				
				Action->StopAction(Instigator);
				return true;
			}
		}
	}
	return false;
}

USAction* USActionComponent::GetAction(TSubclassOf<USAction> ActionClass) {
	for (USAction *Action : Actions) {
		if (Action->IsA(ActionClass)) {
			return Action;
		}
	}
	return nullptr;
}

void USActionComponent::ServerStartAction_Implementation(AActor* Instigator, FName ActionName) {
	StartActionByName(Instigator, ActionName);
}

void USActionComponent::ServerStopAction_Implementation(AActor* Instigator, FName ActionName) {
	StopActionByName(Instigator, ActionName);
}

bool USActionComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) {
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
	for (USAction *Action : Actions) {
		if (Action) {
			WroteSomething |= Channel->ReplicateSubobject(Action, *Bunch, *RepFlags);
		}
	}

	return WroteSomething;
}

void USActionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USActionComponent, Actions);
}
