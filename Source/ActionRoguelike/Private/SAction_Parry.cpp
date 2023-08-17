// Fill out your copyright notice in the Description page of Project Settings.


#include "SAction_Parry.h"

USAction_Parry::USAction_Parry() {
	ParryDuration = 10.0f;
}

void USAction_Parry::StartAction_Implementation(AActor* Instigator) {
	Super::StartAction_Implementation(Instigator);

	FTimerHandle TimerHandle_StopAction;
	FTimerDelegate Delegate;
	Delegate.BindUFunction(this, "DurationDelay_TimeElapsed", Instigator);
	// GetWorld() is override in SAction.
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_StopAction, Delegate, ParryDuration, false);
}

void USAction_Parry::DurationDelay_TimeElapsed(AActor* Instigator) {
	// Same in SAction_ProjectileAttack, we may override StopAction in blueprint, so we can't use StopAction_Implement.
	StopAction(Instigator);
}
