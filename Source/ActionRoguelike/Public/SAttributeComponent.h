// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SAttributeComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnAttributeChanged, AActor*, InstigatorActor,
	USAttributeComponent*, OwningComp, float, NewAttribute, float, Delta);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONROGUELIKE_API USAttributeComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	USAttributeComponent();

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool ApplyHealthChange(AActor *InstigatorActor, float Delta);

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	void ApplyRageChange(AActor *InstigatorActor, float Delta);

	UFUNCTION(BlueprintCallable)
	bool IsInjured();

	UFUNCTION(BlueprintCallable)
	float GetHealth();

	UFUNCTION(BlueprintCallable)
	float GetHealthMax();

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChanged OnRageChanged;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	static USAttributeComponent* GetAttributes(AActor *FromActor);

	UFUNCTION(BlueprintCallable, Category = "Attributes", meta = (DisplayName = "IsAlive"))
	static bool IsActorAlive(AActor *FromActor);

	UFUNCTION(BlueprintCallable)
	bool Kill(AActor* InstigatorActor);

	UFUNCTION(BlueprintCallable)
	float GetRage() const;

	UFUNCTION(BlueprintCallable)
	float GetRageMax() const;
	
protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Attributes")
	float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Attributes")
	float HealthMax;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Attributes")
	float Rage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Attributes")
	float RageMax;

	UFUNCTION(BlueprintCallable)
	bool IsAlive();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastHealthChanged(AActor *InstigatorActor, float NewHealth, float Delta);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRageChanged(AActor *InstigatorActor, float NewRage, float Delta);
};
