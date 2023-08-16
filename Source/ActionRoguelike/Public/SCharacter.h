// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SActionComponent.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

class USAttributeComponent;
class USInteractionComponent;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
	class ACTIONROGUELIKE_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASCharacter();

	void MoveForward(float val);

	void MoveRight(float val);

	void SprintStart();
	
	void SprintStop();
	
	void PrimaryAttack();

	void DashAttack();
	
	void BlackholeAttack();

	void PrimaryInteract();
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent *SpringArmComp;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent *CameraComp;

	UPROPERTY(VisibleAnywhere)
	USInteractionComponent *InteractionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USAttributeComponent *AttributeComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Actions")
	USActionComponent *ActionComp;

	UFUNCTION()
	void OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp,
		float NewHealth, float Delta);

	virtual void PostInitializeComponents() override;

	UFUNCTION(Exec)
	bool HealSelf(float Amount = 100.0f);

	virtual FVector GetPawnViewLocation() const override;
};
