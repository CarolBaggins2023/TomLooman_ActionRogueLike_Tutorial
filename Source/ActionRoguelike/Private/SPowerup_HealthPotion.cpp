// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerup_HealthPotion.h"

#include "SAttributeComponent.h"
#include "SPlayerState.h"

// Sets default values
ASPowerup_HealthPotion::ASPowerup_HealthPotion()
{
	// MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("MeshComp");
	// RootComponent = MeshComp;

	HealingAmount = 50.0f;

	CreditCost = 50;
}

void ASPowerup_HealthPotion::Interact_Implementation(APawn* InstigatorPawn) {
	if (!ensure(InstigatorPawn)) {
		return;
	}

	USAttributeComponent *AttributeComp = Cast<USAttributeComponent>(
		InstigatorPawn->GetComponentByClass(USAttributeComponent::StaticClass())
		);
	if (ensure(AttributeComp) && AttributeComp->IsInjured()) {
		ASPlayerState *PlayerState = InstigatorPawn->GetPlayerState<ASPlayerState>();
		if (ensure(PlayerState)) {
			// Only when the credits are removed, then execute healing.
			if (PlayerState->RemoveCredits(CreditCost) && AttributeComp->ApplyHealthChange(this, HealingAmount)) {
				HideAndCooldownPowerup();
			}
		}
	}
}
