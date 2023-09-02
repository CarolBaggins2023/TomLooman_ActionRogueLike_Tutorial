// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameplayFunctionLibrary.h"

#include "SAttributeComponent.h"


bool USGameplayFunctionLibrary::ApplyDamage(AActor* DamageCauser, AActor* TargetActor, float DamageAmount) {
	if (DamageCauser == TargetActor) {
		return false;
	}
	
	USAttributeComponent *AttributeComp = USAttributeComponent::GetAttributes(TargetActor);
	if (AttributeComp) {
		return AttributeComp->ApplyHealthChange(DamageCauser, -DamageAmount);
	}
	return false;
}

bool USGameplayFunctionLibrary::ApplyDirectionDamage(AActor* DamageCauser, AActor* TargetActor, float DamageAmount,
	const FHitResult& HitResult) {
	if (ApplyDamage(DamageCauser, TargetActor, DamageAmount)) {
		UPrimitiveComponent *HitComp = HitResult.GetComponent();
		if (HitComp && HitComp->IsSimulatingPhysics(HitResult.BoneName)) {
			HitComp->AddImpulseAtLocation(-HitResult.ImpactNormal * 100000.0f, HitResult.ImpactPoint, HitResult.BoneName);
		}
		// Magic projectile's explosion depends on the return of this function.
		// Because Minions' simulating physics is only opened when it is killed, return true can not be put in the inner if statement.
		return true;
	}
	return false;
}
