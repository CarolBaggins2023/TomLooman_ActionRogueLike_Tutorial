// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameModeBase.h"

#include "EngineUtils.h"
#include "AI/SAICharacter.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "GameFramework/GameSession.h"

ASGameModeBase::ASGameModeBase() {
	SpawnTimerInterval = 2.0f;

	MaxBotCount = 10;
}

void ASGameModeBase::StartPlay() {
	Super::StartPlay();

	// SetTimer in loop.
	GetWorldTimerManager().SetTimer(TimerHandle_SpawnBots, this, &ASGameModeBase::SpawnBotTimerElapsed, SpawnTimerInterval, true);
}

void ASGameModeBase::SpawnBotTimerElapsed() {
	UEnvQueryInstanceBlueprintWrapper *QueryInstance = UEnvQueryManager::RunEQSQuery(this, SpawnBotQuery, this, EEnvQueryRunMode::RandomBest5Pct, nullptr);

	if (ensure(QueryInstance)) {
		// The delegate instance is OnQueryFinishedEvent, but it is protected, so we get it by a public function.
		QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &ASGameModeBase::OnQueryCompleted);
	}
}

void ASGameModeBase::OnQueryCompleted(UEnvQueryInstanceBlueprintWrapper *QueryInstance, EEnvQueryStatus::Type QueryStatus) {
	if (QueryStatus != EEnvQueryStatus::Success) {
		UE_LOG(LogTemp, Warning, TEXT("Spawn Bot EQS Query Failed!"));
		return;
	}

	if (IsValid(DifficultyCurve)) {
		MaxBotCount = DifficultyCurve->GetFloatValue(GetWorld()->TimeSeconds);
	}
	
	int32 NrOfAliveBots = 0;
	// It looks like iterating a vector of ASAICharacters' pointers. 
	for (TActorIterator<ASAICharacter> Iter(GetWorld()); Iter; ++Iter) {
		ASAICharacter *Bot = *Iter;
		if (Bot->IsAlive()) {
			++NrOfAliveBots;
		}

		if (NrOfAliveBots >= MaxBotCount) {
			return;
		}
	}
	
	TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();
	if (Locations.IsValidIndex(0)) {
		FActorSpawnParameters SpawnParams;
		// The spawned actors seems to have collisions with the floor, which will stop their spawn.
		// So, we need to raise them a bit or use parameters to force them to spawn.
		// Locations[0].Z += 1000.0f;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		GetWorld()->SpawnActor<AActor>(MinionClass, Locations[0], FRotator::ZeroRotator, SpawnParams);
	}
}
