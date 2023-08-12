// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SAttributeComponent.h"
#include "GameFramework/Actor.h"
#include "SExplosiveBarrel.generated.h"

class URadialForceComponent;

UCLASS()
class ACTIONROGUELIKE_API ASExplosiveBarrel : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASExplosiveBarrel();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> MeshComp;

	// Note that VisibleAnyWhere refers to being visible in the "Details Panel",
	// not visible in the "Blueprint Graph".
	// For that we need to use "BlueReadOnly".
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<URadialForceComponent> ForceComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USAttributeComponent *AttributeComp;

	virtual void PostInitializeComponents() override;

	UFUNCTION()
	void OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp,
		float NewHealth, float Delta);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Why? What will happen if there is no UFUNTION?
	// Because in .cpp, we bind this function with this class by "dynamic multi-cast delegate",
	// which means it will be used in Blueprint.
	// So, it must be marked with UFUNCTION in order to 'bind' the event.
	// The meaning of its parameters are ignored now for simplicity.
	// UFUNCTION()
	// void OnActorHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	// 	FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
