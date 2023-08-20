// Fill out your copyright notice in the Description page of Project Settings.


#include "SMagicProjectile.h"


#include "SActionEffect.h"
#include "SAttributeComponent.h"
#include "SCharacter.h"
#include "SGameplayFunctionLibrary.h"
#include "Chaos/GeometryParticlesfwd.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASMagicProjectile::ASMagicProjectile()
{
	
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ASMagicProjectile::OnActorOverlap);
	
	MoveComp->InitialSpeed = 3000.0f;

	Damage = 50.0f;
}

void ASMagicProjectile::BeginPlay() {
	Super::BeginPlay();
}

void ASMagicProjectile::OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                       UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (IsValid(OtherActor) && OtherActor != GetInstigator()) {
		USActionComponent *ActionComp = Cast<USActionComponent>(OtherActor->GetComponentByClass(USActionComponent::StaticClass()));
		if (ActionComp && ActionComp->ActiveGameplayTags.HasTag(ParryTag)) {
			// We can do this as we have 'MoveComp->bRotationFollowsVelocity = true' in SProjectileBase.
			MoveComp->Velocity = -MoveComp->Velocity;

			// Now the instigator of the projectile should be the one who parried it.
			SetInstigator(Cast<APawn>(OtherActor));
			
			// We don't want the projectile explode, or it is useless.
			return;
		}
		
		if (USGameplayFunctionLibrary::ApplyDirectionDamage(GetInstigator(), OtherActor, Damage, SweepResult)) {
			Explode();

			if (BurningActionClass && ActionComp) {
				ActionComp->AddAction(GetInstigator(), BurningActionClass);
			}
		}
	}
	
}