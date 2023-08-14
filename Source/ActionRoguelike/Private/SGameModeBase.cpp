// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameModeBase.h"

#include "EngineUtils.h"
#include "SCharacter.h"
#include "AI/SAICharacter.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "GameFramework/GameSession.h"

static TAutoConsoleVariable<bool> CVarSpawnBots(TEXT("su.SpawnBots"), true, TEXT("Enable spawning of bots vias timer."));

ASGameModeBase::ASGameModeBase() {
	SpawnTimerInterval = 2.0f;

	MaxBotCount = 10;
}

void ASGameModeBase::StartPlay() {
	Super::StartPlay();

	// SetTimer in loop.
	GetWorldTimerManager().SetTimer(TimerHandle_SpawnBots, this, &ASGameModeBase::SpawnBotTimerElapsed, SpawnTimerInterval, true);
}

void ASGameModeBase::RespawnPlayerElapsed(AController* PlayerController) {
	if (ensure(PlayerController)) {
		PlayerController->UnPossess();
		
		RestartPlayer(PlayerController);
	}
}

void ASGameModeBase::OnActorKilled(AActor* VictimActor, AActor* Killer) {
	// Check whether the killed one is player.
	ASCharacter *Player = Cast<ASCharacter>(VictimActor);
	if (Player) {
		FTimerHandle TimerHandle_RespawnDelay;

		FTimerDelegate Delegate;
		Delegate.BindUFunction(this, "RespawnPlayerElapsed", Player->GetController());

		float RespawnDelay = 3.0f;
		GetWorldTimerManager().SetTimer(TimerHandle_RespawnDelay, Delegate, RespawnDelay, false);
	}

	UE_LOG(LogTemp, Log, TEXT("OnActorKilled: VictimActor: %s, Killer: %s"), *GetNameSafe(VictimActor), *GetNameSafe(Killer));
}

void ASGameModeBase::SpawnBotTimerElapsed() {
	if (!CVarSpawnBots.GetValueOnGameThread()) {
		UE_LOG(LogTemp, Warning, TEXT("Bot spawning disable via cvar 'CVarSpawnBots'"));
		return;
	}
	
	if (IsValid(DifficultyCurve)) {
		MaxBotCount = DifficultyCurve->GetFloatValue(GetWorld()->TimeSeconds);
	}
	
	int32 NrOfAliveBots = 0;
	// It looks like iterating a vector of ASAICharacters' pointers. 
	for (TActorIterator<ASAICharacter> Iter(GetWorld()); Iter; ++Iter) {
		ASAICharacter *Bot = *Iter;
		USAttributeComponent *AttributeComp = USAttributeComponent::GetAttributes(Bot);
		if (AttributeComp && Bot->IsAlive()) {
			++NrOfAliveBots;
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Find %i alive bots."), NrOfAliveBots);
	
	if (NrOfAliveBots >= MaxBotCount) {
		UE_LOG(LogTemp, Log, TEXT("At maximum bot capacity. Skipping bot spawn."));
		return;
	}
	
	UEnvQueryInstanceBlueprintWrapper *QueryInstance = UEnvQueryManager::RunEQSQuery(this, SpawnBotQuery, this, EEnvQueryRunMode::RandomBest5Pct, nullptr);

	if (ensure(QueryInstance)) {
		// The delegate instance is OnQueryFinishedEvent, but it is protected, so we get it by a public function.
		QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &ASGameModeBase::OnQueryCompleted);
	}
}

void ASGameModeBase::KillAll(AActor* InstigatorActor) {
	for (TActorIterator<ASAICharacter> Iter(GetWorld()); Iter; ++Iter) {
		ASAICharacter *Bot = *Iter;
		USAttributeComponent *AttributeComp = USAttributeComponent::GetAttributes(Bot);
		if (AttributeComp && Bot->IsAlive()) {
			AttributeComp->Kill(InstigatorActor);
		}
	}
}

void ASGameModeBase::OnQueryCompleted(UEnvQueryInstanceBlueprintWrapper *QueryInstance, EEnvQueryStatus::Type QueryStatus) {
	if (QueryStatus != EEnvQueryStatus::Success) {
		UE_LOG(LogTemp, Warning, TEXT("Spawn Bot EQS Query Failed!"));
		return;
	}
	
	TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();
	if (Locations.IsValidIndex(0)) {
		FActorSpawnParameters SpawnParams;
		// The spawned actors seems to have collisions with the floor, which will stop their spawn.
		// So, we need to raise them a bit or use parameters to force them to spawn.
		// Locations[0].Z += 1000.0f;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		GetWorld()->SpawnActor<AActor>(MinionClass, Locations[0], FRotator::ZeroRotator, SpawnParams);

		DrawDebugSphere(GetWorld(), Locations[0], 50.0f, 20, FColor::Blue, false, 60.0f);
	}
}
