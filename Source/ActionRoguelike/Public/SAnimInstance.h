// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SAnimInstance.generated.h"

class USActionComponent;
/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API USAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:

	/* Based on GameplayTag data, judging whether the Pawn is stunned. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	bool bIsStunned;

	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	USActionComponent *ActionComp;

	/* Like BeginPlay() of Actors. */
	virtual void NativeInitializeAnimation() override;

	/* Like Tick(). */
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
};
