// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BlueprintNodeHelpers.h"
#include "GameplayTagContainer.h"
#include "SAction.h"
#include "Components/ActorComponent.h"
#include "SActionComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONROGUELIKE_API USActionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	USActionComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Actions")
	void AddAction(AActor *Instigator, TSubclassOf<USAction> ActionClass);

	UFUNCTION(BlueprintCallable, Category = "Actions")
	void RemoveAction(USAction *ActionToRemove);

	UFUNCTION(BlueprintCallable, Category = "Actions")
	bool StartActionByName(AActor *Instigator, FName ActionName);

	UFUNCTION(BlueprintCallable, Category = "Actions")
	bool StopActionByName(AActor *Instigator, FName ActionName);

	virtual bool ReplicateSubobjects(class UActorChannel *Channel, class FOutBunch *Bunch, FReplicationFlags *RepFlags) override;

	UFUNCTION(BlueprintCallable, Category = "Actions")
	USAction* GetAction(TSubclassOf<USAction> ActionClass);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tags")
	FGameplayTagContainer ActiveGameplayTags;
	
protected:

	UPROPERTY(EditAnywhere, Category = "Actions")
	TArray<TSubclassOf<USAction>> DefaultActions;

	UPROPERTY(Replicated)
	TArray<USAction*> Actions;

	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable)
	void ServerStartAction(AActor *Instigator, FName ActionName);

	UFUNCTION(Server, Reliable)
	void ServerStopAction(AActor *Instigator, FName ActionName);
};
