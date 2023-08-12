// Fill out your copyright notice in the Description page of Project Settings.


#include "SHealthPotion.h"

#include "SAttributeComponent.h"

// Sets default values
ASHealthPotion::ASHealthPotion()
{
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("MeshComp");
	RootComponent = MeshComp;

	HealingAmount = 50.0f;
}

void ASHealthPotion::Interact_Implementation(APawn* InstigatorPawn) {
	if (!ensure(InstigatorPawn)) {
		return;
	}

	USAttributeComponent *AttributeComp = Cast<USAttributeComponent>(
		InstigatorPawn->GetComponentByClass(USAttributeComponent::StaticClass())
		);
	if (ensure(AttributeComp) && AttributeComp->IsInjured()) {
		bool bIsActualApplied = AttributeComp->ApplyHealthChange(HealingAmount);
		if (bIsActualApplied) {
			HideAndCooldownPowerup();
		}
	}
}
