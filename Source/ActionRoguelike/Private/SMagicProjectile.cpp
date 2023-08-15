// Fill out your copyright notice in the Description page of Project Settings.


#include "SMagicProjectile.h"

#include "SAttributeComponent.h"
#include "SCharacter.h"
#include "SGameplayFunctionLibrary.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

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
	// if (IsValid(OtherActor) && OtherActor != GetInstigator()) {
	// 	USAttributeComponent *AttributeComp = USAttributeComponent::GetAttributes(OtherActor);
	// 	if (IsValid(AttributeComp)) {
	// 		AttributeComp->ApplyHealthChange(GetInstigator(), -1.0f * Damage);
	//
	// 		Explode();
	// 	}
	// }
	if (USGameplayFunctionLibrary::ApplyDirectionDamage(GetInstigator(), OtherActor, Damage, SweepResult)) {
		Explode();
	}
}