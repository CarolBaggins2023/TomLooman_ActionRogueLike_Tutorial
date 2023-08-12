// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SProjectileBase.generated.h"

class UProjectileMovementComponent;
class USphereComponent;

// An abstract class.
UCLASS(Abstract)
class ACTIONROGUELIKE_API ASProjectileBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASProjectileBase();

protected:
	// VFX refers to 'visual effects'.
	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	UParticleSystem *ImpactVFX;

	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	TSubclassOf<UCameraShakeBase> CameraShake;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent *SphereComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UParticleSystemComponent *EffectComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UProjectileMovementComponent *MoveComp;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	UAudioComponent *FlightSound;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	UAudioComponent *ImpactSound;
	
	UFUNCTION()
	virtual void OnActorHit(UPrimitiveComponent *HitComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp,
		FVector NormalImpulse, const FHitResult &Hit);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Explode();

	virtual void PostInitializeComponents() override;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
