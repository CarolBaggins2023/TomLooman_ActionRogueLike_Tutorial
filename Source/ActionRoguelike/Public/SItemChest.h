// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SGameplayInterface.h"
#include "GameFramework/Actor.h"
#include "SItemChest.generated.h"

UCLASS()
class ACTIONROGUELIKE_API ASItemChest : public AActor, public ISGameplayInterface
{
	GENERATED_BODY()

public:

	// Sets default values for this actor's properties
	ASItemChest();

	virtual void Interact_Implementation(APawn *InstigatorPawn) override;
	
	UPROPERTY(EditAnywhere)
	float TargetPitch;

protected:

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent *BaseMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent *LidMesh;

	// RepNotifies are only automatically triggered for clients (in C++ !!!).
	// But they are automatically triggered in both clients and servers in Blueprint.
	UFUNCTION()
	void OnRep_LidOpened();

	UPROPERTY(ReplicatedUsing = "OnRep_LidOpened", BlueprintReadOnly) // RepNotify
	bool bLidOpened;

};
