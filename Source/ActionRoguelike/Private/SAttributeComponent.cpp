// Fill out your copyright notice in the Description page of Project Settings.


#include "SAttributeComponent.h"

#include "SCharacter.h"
#include "SGameModeBase.h"

static TAutoConsoleVariable<float> CVarDamageMultiplier(TEXT("su.DamageMultiplier"), 1.0f, TEXT("Global damage modifier for attribute component."), ECVF_Cheat);

// Sets default values
USAttributeComponent::USAttributeComponent()
{
	Health = 100.0f;

	HealthMax = 100.0f;
}

bool USAttributeComponent::ApplyHealthChange(AActor *InstigatorActor, float Delta) {
	if (Delta < 0.0f && !GetOwner()->CanBeDamaged()) {
		FString Msg = FString::Printf(TEXT("%s can't be damaged."), *GetNameSafe(GetOwner()));
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::White, Msg);
		
		return false;
	}

	if (Delta < 0.0f) {
		Delta *= CVarDamageMultiplier.GetValueOnGameThread();
	}
	
	float OldHealth = Health;
	
	Health = FMath::Clamp(Health + Delta, 0, HealthMax);

	float ActualDelta = Health - OldHealth;
	
	OnHealthChanged.Broadcast(InstigatorActor, this, Health, ActualDelta);

	if (ActualDelta < 0.0f && Health <= 0.0f) {
		ASGameModeBase *GM = GetWorld()->GetAuthGameMode<ASGameModeBase>();
		if (GM) {
			GM->OnActorKilled(GetOwner(), InstigatorActor);
		}
	}
	
	return ActualDelta != 0;
}

bool USAttributeComponent::IsAlive() {
	return Health > 0.0f;
}

bool USAttributeComponent::Kill(AActor* InstigatorActor) {
	return ApplyHealthChange(InstigatorActor, -HealthMax);
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

USAttributeComponent* USAttributeComponent::GetAttributes(AActor* FromActor) {
	if (FromActor) {
		return Cast<USAttributeComponent>(FromActor->GetComponentByClass(StaticClass()));
	}
	return nullptr;
}

bool USAttributeComponent::IsActorAlive(AActor* Actor) {
	USAttributeComponent *AttributeComp = GetAttributes(Actor);
	if (AttributeComp) {
		return AttributeComp->IsAlive();
	}
	return false;
}
