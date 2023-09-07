// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerupActor.h"

#include "Animation/AnimInstanceProxy.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"

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

	bIsActive = true;
	
	// If server spawns an replicated actor, then the server will tell all clients to spawn
	// a copy of this actor, of the same class and same transformation.
	// The same when it comes to delete an replicated actor.
	SetReplicates(true);
}

void ASPowerupActor::Interact_Implementation(APawn* InstigatorPawn) {
	// logic in derived classes
}

FText ASPowerupActor::GetInteractText_Implementation(APawn* InstigatorPawn) {
	return FText::GetEmpty();
}

void ASPowerupActor::ShowPowerup() {
	SetPowerupState(true);
}

void ASPowerupActor::HideAndCooldownPowerup() {
	SetPowerupState(false);

	GetWorldTimerManager().SetTimer(TimerHandle_RespawnTimer, this, &ASPowerupActor::ShowPowerup, RespawnTime);
}

void ASPowerupActor::SetPowerupState(bool bNewIsActive) {
	bIsActive = bNewIsActive;

	// Since we call it only on the Server, we should trigger RepNotify automatically.
	OnRep_IsActive();
}

void ASPowerupActor::OnRep_IsActive() {
	// collision
	SetActorEnableCollision(bIsActive);
	
	// visibility and propagate to children
	RootComponent->SetVisibility(bIsActive, true);
}

void ASPowerupActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASPowerupActor, bIsActive);
}
