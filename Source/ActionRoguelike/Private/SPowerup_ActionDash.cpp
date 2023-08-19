// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerup_ActionDash.h"

#include "SActionComponent.h"

ASPowerupActor_ActionDash::ASPowerupActor_ActionDash() {
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("MeshComp");
	RootComponent = MeshComp;
}

void ASPowerupActor_ActionDash::Interact_Implementation(APawn* InstigatorPawn) {
	if (!ensure(InstigatorPawn && DashActionClass)) {
		return;
	}
	
	USActionComponent *ActionComp = Cast<USActionComponent>(InstigatorPawn->GetComponentByClass(USActionComponent::StaticClass()));
	if (ActionComp) {
		if (ActionComp->GetAction(DashActionClass)) {
			FString Msg = FString::Printf(TEXT("Action %s is already known"), *GetNameSafe(DashActionClass));
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, Msg);
			return;
		}

		ActionComp->AddAction(InstigatorPawn, DashActionClass);
		HideAndCooldownPowerup();
	}
}
