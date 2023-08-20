// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameModeBase.h"

#include "EngineUtils.h"
#include "SCharacter.h"
#include "SPlayerState.h"
#include "AI/SAICharacter.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "GameFramework/GameSession.h"

static TAutoConsoleVariable<bool> CVarSpawnBots(TEXT("su.SpawnBots"), false, TEXT("Enable spawning of bots vias timer."));

ASGameModeBase::ASGameModeBase() {
	BotSpawnTimerInterval = 2.0f;

	PowerupSpawnTimerInterval = 10.0f;

	MaxBotCount = 10;

	PlayerStateClass = ASPlayerState::StaticClass();

	CreditsKill = 10;

	DesiredPowerupCount = 10;
	RequiredPowerupDistance = 500.0f;
}

void ASGameModeBase::StartPlay() {
	Super::StartPlay();

	// SetTimer in loop.
	GetWorldTimerManager().SetTimer(TimerHandle_SpawnBots, this, &ASGameModeBase::SpawnBotTimerElapsed, BotSpawnTimerInterval, true);

	if (PowerupClasses.Num() > 0) {
		GetWorldTimerManager().SetTimer(TimerHandle_SpawnPowerup, this, &ASGameModeBase::SpawnPowerupTimerElapsed, PowerupSpawnTimerInterval, true);
	}
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
		QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &ASGameModeBase::OnBotSpawnQueryCompleted);
	}
}

void ASGameModeBase::SpawnPowerupTimerElapsed() {
	UEnvQueryInstanceBlueprintWrapper *QueryInstance = UEnvQueryManager::RunEQSQuery(this, SpawnPowerupQuery, this, EEnvQueryRunMode::RandomBest5Pct, nullptr);

	if (ensure(QueryInstance)) {
		QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &ASGameModeBase::OnPowerupSpawnQueryCompleted);
	}
}

void ASGameModeBase::OnBotSpawnQueryCompleted(UEnvQueryInstanceBlueprintWrapper *QueryInstance, EEnvQueryStatus::Type QueryStatus) {
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

void ASGameModeBase::OnPowerupSpawnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus) {
	if (QueryStatus != EEnvQueryStatus::Success) {
		UE_LOG(LogTemp, Warning, TEXT("Spawn Powerup EQS Query Failed!"));
		return;
	}

	TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();
	TArray<FVector> UsedLocations;
	int32 SpawnCounter = 0;
	while (SpawnCounter < DesiredPowerupCount && Locations.Num() > 0) {

		// Pick a random location.
		int32 RandomLocationIndex = FMath::RandRange(0, Locations.Num() - 1);
		FVector PickedLocation = Locations[RandomLocationIndex];

		// Remove the picked location.
		Locations.RemoveAt(RandomLocationIndex);

		// Check if valid.
		bool bValidLocation = true;
		for (FVector OtherLocation : UsedLocations) {

			// Skip locations which are too close to others.
			float DistanceTo = (PickedLocation - OtherLocation).Size();
			if (DistanceTo < RequiredPowerupDistance) {
				bValidLocation = false;
				break;
			}
		}

		// Fail in validation test.
		if (!bValidLocation) {
			continue;
		}

		// Pick a random powerup class and spawn.
		int32 RandomClassIndex = FMath::RandRange(0, PowerupClasses.Num() - 1);
		TSubclassOf<ASPowerupActor> RandomPowerupClass = PowerupClasses[RandomClassIndex];
		GetWorld()->SpawnActor<AActor>(RandomPowerupClass, PickedLocation, FRotator::ZeroRotator);

		// Keep for checks.
		UsedLocations.Add(PickedLocation);
		++SpawnCounter;
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

	APawn *KillerPawn = Cast<APawn>(Killer);
	if (KillerPawn) {
		ASPlayerState *PlayerState = KillerPawn->GetPlayerState<ASPlayerState>();
		if (PlayerState) {
			PlayerState->AddCredits(CreditsKill);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("OnActorKilled: VictimActor: %s, Killer: %s"), *GetNameSafe(VictimActor), *GetNameSafe(Killer));
}

void ASGameModeBase::RespawnPlayerElapsed(AController* PlayerController) {
	if (ensure(PlayerController)) {
		PlayerController->UnPossess();
		
		RestartPlayer(PlayerController);
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
