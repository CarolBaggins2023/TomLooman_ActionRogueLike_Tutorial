// Fill out your copyright notice in the Description page of Project Settings.


#include "SMagicProjectile.h"

#include "SAttributeComponent.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
ASMagicProjectile::ASMagicProjectile()
{
	
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ASMagicProjectile::OnActorOverlap);
	
	MoveComp->InitialSpeed = 3000.0f;
}

void ASMagicProjectile::BeginPlay() {
	Super::BeginPlay();
}

void ASMagicProjectile::OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                       UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (IsValid(OtherActor) && OtherActor != GetInstigator()) {
		USAttributeComponent *AttributeComp = Cast<USAttributeComponent>(
			OtherActor->GetComponentByClass(USAttributeComponent::StaticClass())
			);
		if (IsValid(AttributeComp)) {
			AttributeComp->ApplyHealthChange(-1.0f * Damage);

			Explode();
		}
	}
}