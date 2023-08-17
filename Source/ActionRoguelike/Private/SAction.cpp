// Fill out your copyright notice in the Description page of Project Settings.


#include "SAction.h"

#include "SActionComponent.h"

void USAction::StartAction_Implementation(AActor* Instigator) {
	UE_LOG(LogTemp, Log, TEXT("Start action %s."), *ActionName.ToString());

	USActionComponent *Comp = GetOwningComponent();
	Comp->ActiveGameplayTags.AppendTags(GrantsTags);

	bIsRunning = true;
}

void USAction::StopAction_Implementation(AActor* Instigator) {
	UE_LOG(LogTemp, Log, TEXT("Stop action %s."), *ActionName.ToString());

	ensureAlways(bIsRunning);

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
	USActionComponent *Comp = Cast<USActionComponent>(GetOuter());
	if (Comp) {
		return Comp->GetWorld();
	}

	return nullptr;
}

USActionComponent* USAction::GetOwningComponent() const {
	return Cast<USActionComponent>(GetOuter());
}
