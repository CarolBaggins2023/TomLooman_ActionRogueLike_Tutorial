// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SAttributeComponent.h"
#include "SWorldUserWidget.h"
#include "GameFramework/Character.h"
#include "SAICharacter.generated.h"

class USActionComponent;
class UPawnSensingComponent;

UCLASS()
class ACTIONROGUELIKE_API ASAICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASAICharacter();

	UFUNCTION(BlueprintCallable)
	bool IsAlive();

	UFUNCTION(BlueprintCallable)
	bool IsLowHealth();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UPawnSensingComponent *PawnSensingComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USAttributeComponent *AttributeComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USActionComponent *ActionComp;

	UPROPERTY(EditDefaultsOnly, Category = "Health")
	float LowHealthThreshold;

	UPROPERTY(VisibleAnywhere, Category = "Effects")
	FName TimeToHitParamName;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> HealthBarWidgetClass;
	
	USWorldUserWidget *ActiveHealthBar;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> PlayerSpottedWidgetClass;

	USWorldUserWidget *PlayerSpottedWidget;

	UFUNCTION()
	void OnPawnSeen(APawn* Pawn);

	UFUNCTION()
	void SetTargetActor(AActor *TargetActor);

	UFUNCTION()
	AActor* GetTargetActor();

	virtual void PostInitializeComponents() override;

	UFUNCTION()
	void OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp,
		float NewHealth, float Delta);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastPawnSeen();
};
