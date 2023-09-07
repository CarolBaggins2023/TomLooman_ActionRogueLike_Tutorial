// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SMonsterData.h"
#include "SPowerupActor.h"
#include "SSaveGame.h"
#include "Engine/DataTable.h"
#include "EnvironmentQuery/EnvQuery.h"
#include "EnvironmentQuery/EnvQueryInstanceBlueprintWrapper.h"
#include "GameFramework/GameModeBase.h"
#include "SGameModeBase.generated.h"

class UDataTable;

USTRUCT(BlueprintType)
struct FMonsterInfoRow : public FTableRowBase {
	GENERATED_BODY()

public:

	FMonsterInfoRow() {
		Weight = 1.0f;
		SpawnCost = 5.0f;
		KillReward = 20.0f;
	}

	/*UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AActor> MonsterClass;*/

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USMonsterData *MonsterData;

	/* Relative chance to pick this monster. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Weight;

	/* Points required by GameMode to spawn this unit. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SpawnCost;

	/* Amount of credits awarded to killer of this unit. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float KillReward;
};

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API ASGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:

	ASGameModeBase();
	
	virtual void StartPlay() override;
	
	virtual void OnActorKilled(AActor *VictimActor, AActor *Killer);

	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	void WriteSaveGame();

	void LoadSaveGame();

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

protected:

	FTimerHandle TimerHandle_SpawnBots;

	FTimerHandle TimerHandle_SpawnPowerup;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float BotSpawnTimerInterval;

	UPROPERTY(EditDefaultsOnly, Category = "Powerup")
	float PowerupSpawnTimerInterval;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UEnvQuery *SpawnBotQuery;

	UPROPERTY(EditDefaultsOnly, Category = "Powerup")
	UEnvQuery *SpawnPowerupQuery;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UDataTable *MonsterTable;
	
	/*UPROPERTY(EditDefaultsOnly, Category = "AI")
	TSubclassOf<AActor> MinionClass;*/

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	int32 MaxBotCount;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UCurveFloat *DifficultyCurve;

	UPROPERTY(EditAnywhere, Category = "PlayerState")
	int32 CreditsKill;

	UPROPERTY(EditDefaultsOnly, Category = "Powerup")
	TArray<TSubclassOf<ASPowerupActor>> PowerupClasses;

	UPROPERTY(EditDefaultsOnly, Category = "Powerup")
	int32 DesiredPowerupCount;

	UPROPERTY(EditDefaultsOnly, Category = "Powerup")
	float RequiredPowerupDistance;

	FString SlotName;

	UPROPERTY()
	USSaveGame *CurrentSaveGame;

	UFUNCTION()
	void OnBotSpawnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);

	UFUNCTION()
	void OnPowerupSpawnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);
	
	UFUNCTION()
	void SpawnBotTimerElapsed();

	UFUNCTION()
	void SpawnPowerupTimerElapsed();

	UFUNCTION(Exec)
	void KillAll(AActor *InstigatorActor);

	UFUNCTION()
	void RespawnPlayerElapsed(AController *PlayerController);
};
