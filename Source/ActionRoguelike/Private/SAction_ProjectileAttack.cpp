// Fill out your copyright notice in the Description page of Project Settings.


#include "SAction_ProjectileAttack.h"

#include "SCharacter.h"
#include "Kismet/GameplayStatics.h"

USAction_ProjectileAttack::USAction_ProjectileAttack() {
	HandSocketName = "Muzzle_01";

	AttackAnimDelay = 0.2f;
}

void USAction_ProjectileAttack::StartAction_Implementation(AActor* Instigator) {
	Super::StartAction_Implementation(Instigator);

	ASCharacter *Character = Cast<ASCharacter>(Instigator);
	if (Character) {
		// Make Character turn to attack direction.
		FRotator ControlRotator = Character->GetControlRotation();
		ControlRotator.Pitch = Character->GetActorRotation().Pitch;
		ControlRotator.Roll = Character->GetActorRotation().Roll;
		Character->SetActorRotation(ControlRotator, ETeleportType::None); 
		
		Character->PlayAnimMontage(AttackAnim);
	
		UGameplayStatics::SpawnEmitterAttached(CastingEffect, Character->GetMesh(), FName(HandSocketName));

		// Spawn projectiles only on the Server and the Server will call Clients to replicate the spawning.
		// Or USAction_ProjectileAttack::AttackDelay_TimeElapsed will be triggered twice.
		if (Character->HasAuthority()) {
			FTimerHandle TimerHandle_AttackDelay;
			FTimerDelegate Delegate;
			Delegate.BindUFunction(this, "AttackDelay_TimeElapsed", Character);
			// GetWorld() is override in SAction.
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_AttackDelay, Delegate, AttackAnimDelay, false);
		}
	}
}

void USAction_ProjectileAttack::AttackDelay_TimeElapsed(ACharacter* InstigatorCharacter) {
	if (ensureAlways(ProjectileClass)) {
		// Get Start and End vector to construct rotator.
		FVector HandLocation = InstigatorCharacter->GetMesh()->GetSocketLocation(FName(HandSocketName));
		FVector Start = HandLocation;
		
		// Get End vector is somewhat troublesome.
		// Perform line trace detection.
		FHitResult Hit;

		// Note that trace start and end are changed.
		FVector TraceStart = InstigatorCharacter->GetPawnViewLocation();
		FVector TraceEnd = TraceStart + InstigatorCharacter->GetControlRotation().Vector() * 5000.0f;
		
		FCollisionObjectQueryParams ObjectQueryParams;
		ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
		ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
		ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);

		FCollisionShape Shape;
		Shape.SetSphere(20.0f);

		// Ignore the player.
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(InstigatorCharacter);

		// GetWorld() is overrided in USAction
		bool bBlockingHit = GetWorld()->SweepSingleByObjectType(Hit, TraceStart, TraceEnd,
			FQuat::Identity, ObjectQueryParams, Shape, QueryParams);
		
		FVector End = bBlockingHit ? Hit.ImpactPoint : TraceEnd;

		// Get the transformation of spawned projectile.
		FRotator ProjectileRotation = FRotationMatrix::MakeFromX(End - Start).Rotator();
		FTransform SpawnTM = FTransform(ProjectileRotation, HandLocation);

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Instigator = InstigatorCharacter;
	
		GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnTM, SpawnParams);
	}

	StopAction(InstigatorCharacter);
}
