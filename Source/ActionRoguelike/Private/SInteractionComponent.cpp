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

	TraceDistance = 500.0f;
	TraceRadius = 30.0f;
	CollisionChannel = ECC_WorldDynamic;
}

void USInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FindBestInteractable();
}

void USInteractionComponent::FindBestInteractable() {
	FVector Start, End;
	AActor *MyOwner = GetOwner();
	FVector EyesLocation;
	FRotator EyesRotation;
	MyOwner->GetActorEyesViewPoint(EyesLocation, EyesRotation);
	Start = EyesLocation;
	End = EyesLocation + EyesRotation.Vector() * TraceDistance;

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(CollisionChannel);
	
	// Sweep needs an array of HitResult, which is quite different from LineTrace.
	TArray<FHitResult> Hits;
	float Radius = TraceRadius;
	FCollisionShape	Shape;
	Shape.SetSphere(Radius);
	bool bBlockingHit = GetWorld()->SweepMultiByObjectType(Hits, Start, End, FQuat::Identity, ObjectQueryParams, Shape);
	FColor LineColor = bBlockingHit ? FColor::Green : FColor::Red;

	// Clear ref before trying to fill.
	FocusActor = nullptr;
	for (FHitResult Hit : Hits) {
		// The process for each Hit is the same.
		AActor *HitActor = Hit.GetActor();
		if (HitActor) {
			// Note: there is USGameplayInterface, not IS...
			if (HitActor->Implements<USGameplayInterface>()) {
				if (CVarDebugDrawInteraction.GetValueOnGameThread()) {
					// We only execute the interface on the first object that is hit.
					DrawDebugSphere(GetWorld(), Hit.ImpactPoint, Radius, 32, LineColor, false, 2.0f);
				}

				FocusActor = HitActor;
				break;
			}
		}
	}

	if (FocusActor) {
		if (DefaultWidgetInstance == nullptr && ensure(DefaultWidgetClass)) {
			DefaultWidgetInstance = CreateWidget<USWorldUserWidget>(GetWorld(), DefaultWidgetClass);
		}

		if (DefaultWidgetInstance) {
			DefaultWidgetInstance->AttachedActor = FocusActor;

			if (!DefaultWidgetInstance->IsInViewport()) {
				DefaultWidgetInstance->AddToViewport();
			}
		}
	} else {
		if (DefaultWidgetInstance) {
			DefaultWidgetInstance->RemoveFromParent();
		}
	}
	
	if (CVarDebugDrawInteraction.GetValueOnGameThread()) {
		DrawDebugLine(GetWorld(), Start, End, LineColor, false, 2.0f, 0, 2.0f);
	}
}

void USInteractionComponent::PrimaryInteract() {
	if (FocusActor == nullptr) {
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, "No FocusActor to interact.");
		return;
	}
	
	APawn *MyPawn = Cast<APawn>(GetOwner());
	ISGameplayInterface::Execute_Interact(FocusActor, MyPawn);
}
