// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SAction.h"
#include "SPowerupActor.h"
#include "SPowerup_ActionDash.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API ASPowerupActor_ActionDash : public ASPowerupActor
{
	GENERATED_BODY()

public:

	// ASPowerupActor_ActionDash();
	
	virtual void Interact_Implementation(APawn* InstigatorPawn) override;

protected:
	
	// UPROPERTY(EditAnywhere, BlueprintReadOnly)
	// UStaticMeshComponent *MeshComp;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<USAction> DashActionClass;
};
