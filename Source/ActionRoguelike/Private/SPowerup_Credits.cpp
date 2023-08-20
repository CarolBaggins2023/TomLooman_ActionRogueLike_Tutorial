// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerup_Credits.h"

#include "SPlayerState.h"


ASPowerup_Credits::ASPowerup_Credits() {

	// MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("MeshComp");
	// RootComponent = MeshComp;
	
	CreditsAmount = 100;

	// override the parameter in parent class
	RespawnTime = 10.0f;
}

void ASPowerup_Credits::Interact_Implementation(APawn* InstigatorPawn) {
	if (!ensure(InstigatorPawn)) {
		return;
	}

	ASPlayerState *PlayerState = InstigatorPawn->GetPlayerState<ASPlayerState>();
	if (PlayerState) {
		PlayerState->AddCredits(CreditsAmount);
		HideAndCooldownPowerup();
	}
}
