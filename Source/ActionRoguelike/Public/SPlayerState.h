// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCreditsChanged, ASPlayerState*, PlayerState, int32, NewCredits, int32, Delta);

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API ASPlayerState : public APlayerState
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnCreditsChanged OnCreditsChanged;

	UFUNCTION(BlueprintCallable, Category = "Credits")
	int GetCredits() const;

	UFUNCTION(BlueprintCallable, Category = "Credits")
	void AddCredits(int32 Delta);

	UFUNCTION(BlueprintCallable, Category = "Credits")
	bool RemoveCredits(int32 Delta);

	UFUNCTION(BlueprintNativeEvent)
	void SavePlayerState(USSaveGame *SaveObject);

	UFUNCTION(BlueprintNativeEvent)
	void LoadPlayerState(USSaveGame *SaveObject);
	
protected:

	UPROPERTY(EditDefaultsOnly, Category = "Credits")
	int32 Credits;
};
