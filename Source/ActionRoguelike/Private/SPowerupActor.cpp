// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerupActor.h"

#include "Animation/AnimInstanceProxy.h"
#include "Components/SphereComponent.h"

// Sets default values
ASPowerupActor::ASPowerupActor()
{
	SphereComp = CreateDefaultSubobject<USphereComponent>("SphereComp");
	SphereComp->SetCollisionProfileName("Powerup");
	RootComponent = SphereComp;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("MeshComp");
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComp->SetupAttachment(RootComponent);

	RespawnTime = 5.0f;

	SetReplicates(true);
}

void ASPowerupActor::Interact_Implementation(APawn* InstigatorPawn) {
	// logic in derived classes
}

void ASPowerupActor::ShowPowerup() {
	SetPowerupState(true);
}

void ASPowerupActor::HideAndCooldownPowerup() {
	SetPowerupState(false);

	GetWorldTimerManager().SetTimer(TimerHandle_RespawnTimer, this, &ASPowerupActor::ShowPowerup, RespawnTime);
}

void ASPowerupActor::SetPowerupState(bool bIsActivate) {
	// collision
	SetActorEnableCollision(bIsActivate);
	
	// visibility and propagate to children
	RootComponent->SetVisibility(bIsActivate, true);
}
