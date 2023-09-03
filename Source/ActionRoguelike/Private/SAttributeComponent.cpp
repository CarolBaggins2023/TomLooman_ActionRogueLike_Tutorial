// Fill out your copyright notice in the Description page of Project Settings.


#include "SAttributeComponent.h"

#include "IntVectorTypes.h"
#include "SCharacter.h"
#include "SGameModeBase.h"
#include "Net/UnrealNetwork.h"

static TAutoConsoleVariable<float> CVarDamageMultiplier(TEXT("su.DamageMultiplier"), 1.0f, TEXT("Global damage modifier for attribute component."), ECVF_Cheat);

// Sets default values
USAttributeComponent::USAttributeComponent()
{
	Health = 100.0f;
	HealthMax = 100.0f;

	Rage = 0.0f;
	RageMax = 100.0f;

	// Function to replicate variables in Component is different. 
	SetIsReplicatedByDefault(true);
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
	float NewHealth = FMath::Clamp(Health + Delta, 0, HealthMax);

	float ActualDelta = NewHealth - OldHealth;

	// Only cause real damage on Server.
	if (GetOwner()->HasAuthority()) {
		Health = NewHealth;

		// When it comes to networking, not wasting information is a big deal.
		if (ActualDelta != 0.0f) {
			// It will be called in MulticastHealthChanged.
			// OnHealthChanged.Broadcast(InstigatorActor, this, Health, ActualDelta);

			// If it is called on a client, it will just be called locally for its own client,
			// and will not be sent to anyone else.
			MulticastHealthChanged(InstigatorActor, Health, ActualDelta);
		}

		// GameMode only exists on Server.
		if (ActualDelta < 0.0f && Health <= 0.0f) {
			ASGameModeBase *GM = GetWorld()->GetAuthGameMode<ASGameModeBase>();
			if (GM) {
				GM->OnActorKilled(GetOwner(), InstigatorActor);
			}
		}
	}
	
	return ActualDelta != 0;
}

void USAttributeComponent::ApplyRageChange(AActor *InstigatorActor, float Delta) {
	Rage = FMath::Clamp(Rage + Delta, 0, RageMax);
	OnRageChanged.Broadcast(InstigatorActor, this, Rage, Delta);
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

float USAttributeComponent::GetRage() const {
	return Rage;
}

float USAttributeComponent::GetRageMax() const {
	return RageMax;
}

void USAttributeComponent::MulticastHealthChanged_Implementation(AActor* InstigatorActor, float NewHealth, float Delta) {
	OnHealthChanged.Broadcast(InstigatorActor, this, NewHealth, Delta);
}

void USAttributeComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USAttributeComponent, Health);
	DOREPLIFETIME(USAttributeComponent, HealthMax);
	// DOREPLIFETIME_CONDITION(USAttributeComponent, HealthMax, COND_OwnerOnly);
}


