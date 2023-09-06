// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPawnChanged, APawn*, NewPawn);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerStateChanged, APlayerState*, NewPlayerState);

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API ASPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> PauseMenuClass;

	UPROPERTY()
	UUserWidget *PauseMenuInstance;

	UPROPERTY(BlueprintAssignable)
	FOnPawnChanged OnPawnChanged;

	/* Listen for incoming PlayerState.
	 * For clients, the PlayerState may be null when initially joining a game,
	 * afterwards player state will not change again as PlayerController maintains the same PlayerState throughout the game. */
	FOnPlayerStateChanged OnPlayerStateReceived;

	virtual void SetPawn(APawn* InPawn) override;

	/* It is called when player controller is ready to begin playing, much later than BeginPlay, and
	 * a good moment to initialize things like UI. Esp. in multiplayer clients where not all data such
	 * as PlayerState may have been received yet. */
	virtual void BeginPlayingState() override;

	UFUNCTION(BlueprintImplementableEvent)
	void BlueprintBeginPlayingState();

	virtual void OnRep_PlayerState() override;

	UFUNCTION(BlueprintCallable)
	void TogglePauseMenu();

	virtual void SetupInputComponent() override;
};
