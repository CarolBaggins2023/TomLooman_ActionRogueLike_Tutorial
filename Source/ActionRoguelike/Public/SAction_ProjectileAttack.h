// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SAction.h"
#include "SAction_ProjectileAttack.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API USAction_ProjectileAttack : public USAction
{
	GENERATED_BODY()

public:

	USAction_ProjectileAttack();

	virtual void StartAction_Implementation(AActor* Instigator) override;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Attack");
	UParticleSystem *CastingEffect;

	UPROPERTY(VisibleAnywhere, Category = "Effects")
	FName HandSocketName;

	UPROPERTY(EditAnywhere, Category = "Attack")
	TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Attack")
	float AttackAnimDelay;

	UPROPERTY(EditAnywhere, Category = "Attack")
	UAnimMontage *AttackAnim;

	// If there is no UFUNCTION macro, there there be an error of "Failed to find function."
	UFUNCTION()
	void AttackDelay_TimeElapsed(ACharacter *InstigatorCharacter);
	
};
