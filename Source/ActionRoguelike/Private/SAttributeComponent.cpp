// Fill out your copyright notice in the Description page of Project Settings.


#include "SAttributeComponent.h"

// Sets default values
USAttributeComponent::USAttributeComponent()
{
	Health = 100.0f;

	HealthMax = 100.0f;
}

bool USAttributeComponent::ApplyHealthChange(float Delta) {
	float OldHealth = Health;
	
	Health = FMath::Clamp(Health + Delta, 0, HealthMax);

	float ActualDelta = Health - OldHealth;
	
	OnHealthChanged.Broadcast(nullptr, this, Health, ActualDelta);
	
	return ActualDelta != 0;
}

bool USAttributeComponent::IsAlive() {
	return Health > 0.0f;
}

bool USAttributeComponent::IsInjured() {
	return Health < HealthMax;
}

float USAttributeComponent::GetHealth() {
	return Health;
}

float USAttributeComponent::GetHealthMax() {
	return HealthMax;
}

