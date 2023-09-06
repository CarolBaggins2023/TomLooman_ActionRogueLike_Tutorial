// Fill out your copyright notice in the Description page of Project Settings.


#include "SActionEffect.h"

#include "SActionComponent.h"
#include "GameFramework/GameStateBase.h"

USActionEffect::USActionEffect() {
	bAutoStart = true;
}

void USActionEffect::StartAction_Implementation(AActor* Instigator) {
	Super::StartAction_Implementation(Instigator);

	// So, if we set Duration to 0, then it will never call StopAction, which means the buff is infinite.
	if (Duration > 0.0f) {
		FTimerDelegate Delegate;
		Delegate.BindUFunction(this, "StopAction", Instigator);

		GetWorld()->GetTimerManager().SetTimer(DurationHandle, Delegate, Duration, false);
	}

	if (Period > 0.0f) {
		FTimerDelegate Delegate;
		Delegate.BindUFunction(this, "ExecutePeriodEffect", Instigator);

		GetWorld()->GetTimerManager().SetTimer(PeriodHandle, Delegate, Period, true);
	}
}

void USActionEffect::StopAction_Implementation(AActor* Instigator) {
	if (GetWorld()->GetTimerManager().GetTimerRemaining(PeriodHandle) < KINDA_SMALL_NUMBER) {
		ExecutePeriodEffect(Instigator);
	}

	Super::StopAction_Implementation(Instigator);

	GetWorld()->GetTimerManager().ClearTimer(DurationHandle);
	GetWorld()->GetTimerManager().ClearTimer(PeriodHandle);

	USActionComponent *Comp = GetOwningComponent();
	if (Comp) {
		// Make use of UE garbage collection system.
		Comp->RemoveAction(this);
	}
}

void USActionEffect::ExecutePeriodEffect_Implementation(AActor* Instigator) {
	// Detail logic is in subclass.
}


float USActionEffect::GetTimeRemaining() const {
	AGameStateBase *GS = GetWorld()->GetGameState<AGameStateBase>();
	if (GS) {
		float EndTime = TimeStarted + Duration;
		return EndTime - GS->GetServerWorldTimeSeconds();
	}

	return Duration;
}
