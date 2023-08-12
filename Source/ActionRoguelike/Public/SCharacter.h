// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
	
	void PrimaryAttack();

	void DashAttack();
	
	void BlackholeAttack();

	void PrimaryInteract();

	void Accelarate();
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent *SpringArmComp;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent *CameraComp;

	UPROPERTY(VisibleAnywhere)
	USInteractionComponent *InteractionComp;

	UPROPERTY(EditDefaultsOnly, Category = "Effects");
	UParticleSystem *MuzzleFlash;

	UPROPERTY(VisibleAnywhere, Category = "Effects")
	FName RightHandSocketName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USAttributeComponent *AttributeComp;

	UPROPERTY(EditAnywhere, Category = "Attack")
	TSubclassOf<AActor> PrimaryProjectileClass;
	
	UPROPERTY(EditAnywhere, Category = "Attack");
	TSubclassOf<AActor> DashProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Attack");
	TSubclassOf<AActor> BlackholeProjectileClass;
	
	UPROPERTY(EditAnywhere, Category = "Attack")
	UAnimMontage* AttackAnim;

	float AttackAnimDelay;

	bool bIsAccelerating;
	
	FTimerHandle TimerHandle_PrimaryAttack;

	FTimerHandle TimerHandle_Dash;

	FTimerHandle TimerHandle_Backhole;

	void TurnToAttackDirection();
	
	void PrimaryAttack_TimeElapsed();

	void DashAttack_TimeElapsed();

	void BlackholeAttack_TimeElapsed();

	void SpawnProjectile(TSubclassOf<AActor> ClassToSpawn);

	UFUNCTION()
	void OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp,
		float NewHealth, float Delta);

	virtual void PostInitializeComponents() override;
	
};
