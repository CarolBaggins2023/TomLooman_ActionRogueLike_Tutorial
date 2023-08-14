// Fill out your copyright notice in the Description page of Project Settings.


#include "SInteractionComponent.h"

#include "CopyTextureShaders.h"
#include "SGameplayInterface.h"

static TAutoConsoleVariable<bool> CVarDebugDrawInteraction(TEXT("su.InteractionDebugDraw"), false, TEXT("Enable debug lines for interact component."), ECVF_Cheat);

// Sets default values for this component's properties
USInteractionComponent::USInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void USInteractionComponent::PrimaryInteract() {
	FVector Start, End;
	AActor *MyOwner = GetOwner();
	FVector EyesLocation;
	FRotator EyesRotation;
	MyOwner->GetActorEyesViewPoint(EyesLocation, EyesRotation);
	Start = EyesLocation;
	End = EyesLocation + EyesRotation.Vector() * 1000;

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);

	// The method based on LineTrace is critical, which means it has the precision of pixel.
	// So, we'd better to use another method based on "sweep".
	/*FHitResult Hit;
	bool bBlockingHit = GetWorld()->LineTraceSingleByObjectType(Hit, Start, End, ObjectQueryParams);
	AActor *HitActor = Hit.GetActor();
	if (HitActor) {
		// Note: there is USGameplayInterface, not IS...
		if (HitActor->Implements<USGameplayInterface>()) {
			APawn *MyPawn = Cast<APawn>(MyOwner);
			ISGameplayInterface::Execute_Interact(HitActor, MyPawn);
		}
	}*/
	
	// Sweep needs an array of HitResult, which is quite different from LineTrace.
	TArray<FHitResult> Hits;
	float Radius = 30.0f;
	FCollisionShape	Shape;
	Shape.SetSphere(Radius);
	bool bBlockingHit = GetWorld()->SweepMultiByObjectType(Hits, Start, End, FQuat::Identity, ObjectQueryParams, Shape);
	FColor LineColor = bBlockingHit ? FColor::Green : FColor::Red;
	for (FHitResult Hit : Hits) {
		// The process for each Hit is the same.
		AActor *HitActor = Hit.GetActor();
		if (HitActor) {
			// Note: there is USGameplayInterface, not IS...
			if (HitActor->Implements<USGameplayInterface>()) {
				APawn *MyPawn = Cast<APawn>(MyOwner);
				ISGameplayInterface::Execute_Interact(HitActor, MyPawn);
				if (CVarDebugDrawInteraction.GetValueOnGameThread()) {
					// We only execute the interface on the first object that is hit.
					DrawDebugSphere(GetWorld(), Hit.ImpactPoint, Radius, 32, LineColor, false, 2.0f);
				}
				break;
			}
		}
	}
	if (CVarDebugDrawInteraction.GetValueOnGameThread()) {
		DrawDebugLine(GetWorld(), Start, End, LineColor, false, 2.0f, 0, 2.0f);
	}
}
