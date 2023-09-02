// Fill out your copyright notice in the Description page of Project Settings.


#include "SAction.h"

#include "SActionComponent.h"
#include "ActionRoguelike/ActionRoguelike.h"
#include "Net/UnrealNetwork.h"

void USAction::Initialize(USActionComponent* NewActionComp) {
	ActionComp = NewActionComp;
}

void USAction::StartAction_Implementation(AActor* Instigator) {
	// UE_LOG(LogTemp, Log, TEXT("Start action %s."), *ActionName.ToString());
	LogOnScreen(this, FString::Printf(TEXT("Started: %s"), *ActionName.ToString()), FColor::Green);

	USActionComponent *Comp = GetOwningComponent();
	Comp->ActiveGameplayTags.AppendTags(GrantsTags);

	bIsRunning = true;
}

void USAction::StopAction_Implementation(AActor* Instigator) {
	// UE_LOG(LogTemp, Log, TEXT("Stop action %s."), *ActionName.ToString());
	LogOnScreen(this, FString::Printf(TEXT("Stopped: %s"), *ActionName.ToString()), FColor::Orange);

	// In multi-players game, it will never be true.
	// See USAction::OnRep_IsRunning().
	// Because if it is triggered on a client, only when bIsRunning == false, StopAction is executed.
	// ensureAlways(bIsRunning);

	USActionComponent *Comp = GetOwningComponent();
	Comp->ActiveGameplayTags.RemoveTags(GrantsTags);

	bIsRunning = false;
}

bool USAction::CanStart_Implementation(AActor* Instigator) {
	// It is used to stop starting 'itself' repeatedly.
	// Adding Action.Attacking to block tags in editor is used to stop other actions which belong to Action.Attacking.
	if (IsRunning()) {
		return false;
	}
	
	USActionComponent *Comp = Cast<USActionComponent>(GetOuter());
	if (Comp->ActiveGameplayTags.HasAny(BlockTags)) {
		return false;
	}
	return true;
}

bool USAction::IsRunning() {
	return bIsRunning;
}

UWorld* USAction::GetWorld() const {
	// Outer is set in NewObject in USActionComponent::AddAction
	AActor *Actor = Cast<AActor>(GetOuter());
	if (Actor) {
		return Actor->GetWorld();
	}

	return nullptr;
}

USActionComponent* USAction::GetOwningComponent() const {
	// AActor* Actor = Cast<AActor>(GetOuter());
	// return Actor->GetComponentByClass(USActionComponent::StaticClass());
	
	return ActionComp;
}

void USAction::OnRep_IsRunning() {
	if (bIsRunning) {
		StartAction(nullptr);
	} else {
		StopAction(nullptr);
	}
}

void USAction::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USAction, bIsRunning);
	DOREPLIFETIME(USAction, ActionComp);
}
