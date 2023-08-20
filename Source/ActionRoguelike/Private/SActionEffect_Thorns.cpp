// Fill out your copyright notice in the Description page of Project Settings.


#include "SActionEffect_Thorns.h"

#include "SAttributeComponent.h"
#include "SCharacter.h"
#include "SGameplayFunctionLibrary.h"
#include "Slate/SGameLayerManager.h"

USActionEffect_Thorns::USActionEffect_Thorns() {
	// for infinite buff time
	Duration = 0.0f;
	Period = 0.0f;

	ReflectRate = 1.0f;
}

void USActionEffect_Thorns::StartAction_Implementation(AActor* Instigator) {
	Super::StartAction_Implementation(Instigator);

	USActionComponent *Comp = GetOwningComponent();
	AActor *OwningActor = Cast<ASCharacter>(Comp->GetOwner());
	USAttributeComponent *AttributeComp = USAttributeComponent::GetAttributes(OwningActor);
	if (AttributeComp) {
		AttributeComp->OnHealthChanged.AddDynamic(this, &USActionEffect_Thorns::OnHealthChanged);
	}
}

void USActionEffect_Thorns::StopAction_Implementation(AActor* Instigator) {
	Super::StopAction_Implementation(Instigator);

	// We need to remove the bind with the delegate.
	// Or even we have stopped action, the thorns effect still remains.
	USActionComponent *Comp = GetOwningComponent();
	AActor *OwningActor = Cast<ASCharacter>(Comp->GetOwner());
	USAttributeComponent *AttributeComp = USAttributeComponent::GetAttributes(OwningActor);
	if (AttributeComp) {
		AttributeComp->OnHealthChanged.RemoveDynamic(this, &USActionEffect_Thorns::OnHealthChanged);
	}
}

void USActionEffect_Thorns::OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewAttribute, float Delta) {

	USActionComponent *Comp = GetOwningComponent();
	AActor *OwningActor = Comp->GetOwner();

	if (Delta < 0.0f && OwningActor != InstigatorActor) {
		float ReflectDamage = FMath::Abs(FMath::RoundToInt(Delta * ReflectRate));
		// Maybe it is better to make the damage causer nullptr? 
		// USGameplayFunctionLibrary::ApplyDamage(OwningActor, InstigatorActor, ReflectDamage);
		USGameplayFunctionLibrary::ApplyDamage(nullptr, InstigatorActor, ReflectDamage);
	}
}
