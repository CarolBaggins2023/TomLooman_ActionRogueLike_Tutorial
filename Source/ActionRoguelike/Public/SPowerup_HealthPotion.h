// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SPowerupActor.h"
#include "GameFramework/Actor.h"
#include "SPowerup_HealthPotion.generated.h"

UCLASS()
class ACTIONROGUELIKE_API ASPowerup_HealthPotion : public ASPowerupActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASPowerup_HealthPotion();
	
	virtual void Interact_Implementation(APawn* InstigatorPawn) override;

	virtual FText GetInteractText_Implementation(APawn* Instigator) override;

protected:
	
	// UPROPERTY(VisibleAnywhere)
	// UStaticMeshComponent *MeshComp;

	UPROPERTY(EditAnywhere)
	float HealingAmount;

	UPROPERTY(EditAnywhere)
	int32 CreditCost;
};
