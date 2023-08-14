// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerup_Credits.h"

#include "SPlayerState.h"


ASPowerup_Credits::ASPowerup_Credits() {

	CreditsAmount = 80;

	RespawnTime = 10.0f;
}

void ASPowerup_Credits::Interact_Implementation(APawn* InstigatorPawn) {
	if (!ensure(InstigatorPawn)) {
		return;
	}

	ASPlayerState *PlayerState = InstigatorPawn->GetPlayerState<ASPlayerState>();
	if (ensure(PlayerState)) {
		PlayerState->AddCredits(CreditsAmount);
		HideAndCooldownPowerup();
	}
}
