// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SAction.h"
#include "SAction_Parry.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API USAction_Parry : public USAction
{
	GENERATED_BODY()

public:

	USAction_Parry();
	
	virtual void StartAction_Implementation(AActor* Instigator) override;

protected:

	UFUNCTION()
	void DurationDelay_TimeElapsed(AActor* Instigator);
	
	UPROPERTY(EditAnywhere, Category = "Actions")
	float ParryDuration;
};
