// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameModeBase.h"

#include "EngineUtils.h"
#include "SCharacter.h"
#include "SItemChest.h"
#include "SPlayerState.h"
#include "ActionRoguelike/ActionRoguelike.h"
#include "AI/SAICharacter.h"
#include "Engine/AssetManager.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "GameFramework/GameSession.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"

static TAutoConsoleVariable<bool> CVarSpawnBots(TEXT("su.SpawnBots"), false, TEXT("Enable spawning of bots vias timer."));
static TAutoConsoleVariable<bool> CVarSpawnPowerups(TEXT("su.SpawnPowerups"), false, TEXT("Enable spawning of powerups vias timer."));

ASGameModeBase::ASGameModeBase() {
	BotSpawnTimerInterval = 2.0f;

	PowerupSpawnTimerInterval = 10.0f;

	MaxBotCount = 10;

	PlayerStateClass = ASPlayerState::StaticClass();

	CreditsKill = 10;

	DesiredPowerupCount = 10;
	RequiredPowerupDistance = 500.0f;

	SlotName = "SaveGame01";
}

void ASGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) {
	Super::InitGame(MapName, Options, ErrorMessage);

	FString SelectedSaveSlot = UGameplayStatics::ParseOption(Options, "SaveGame");
	if (SelectedSaveSlot.Len() > 0) {
		SlotName = SelectedSaveSlot;
	}
	
	LoadSaveGame();
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
	if (!CVarSpawnPowerups.GetValueOnGameThread()) {
		UE_LOG(LogTemp, Warning, TEXT("Powerup spawning disable via cvar 'CVarSpawnPowers'"));
		return;
	}

	
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
		
		// The spawned actors seems to have collisions with the floor, which will stop their spawn.
		// So, we need to raise them a bit or use parameters to force them to spawn.
		// Locations[0].Z += 1000.0f;

		if (MonsterTable) {
			TArray<FMonsterInfoRow*> Rows;
			MonsterTable->GetAllRows("", Rows);
			
			int32 RandomIndex = FMath::RandRange(0, Rows.Num() - 1);
			FMonsterInfoRow *SelectedRow = Rows[RandomIndex];

			UAssetManager *Manager = UAssetManager::GetIfValid();
			if (Manager) {
				LogOnScreen(this, "Loading monsters...", FColor::Green);
				
				TArray<FName> Bundles;
				FStreamableDelegate Delegate = FStreamableDelegate::CreateUObject(this, &ASGameModeBase::OnMonsterLoaded, SelectedRow->MonsterId, Locations[0]);
				Manager->LoadPrimaryAsset(SelectedRow->MonsterId, Bundles, Delegate);
			}
		}
	}
}

void ASGameModeBase::OnMonsterLoaded(FPrimaryAssetId LoadedId, FVector SpawnLocation) {
	LogOnScreen(this, "Finish loading.", FColor::Green);
	
	UAssetManager *Manager = UAssetManager::GetIfValid();
	if (Manager) {
		USMonsterData *MonsterData = Cast<USMonsterData>(Manager->GetPrimaryAssetObject(LoadedId));
		if (MonsterData) {
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			
			AActor *NewBot = GetWorld()->SpawnActor<AActor>(MonsterData->MonsterClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
			// DrawDebugSphere(GetWorld(), Locations[0], 50.0f, 20, FColor::Blue, false, 60.0f);
			LogOnScreen(this, FString::Printf(TEXT("Spawn enemy: %s (%s)"), *GetNameSafe(NewBot), *GetNameSafe(MonsterData)));

			// Grant special actions, buffs etc.
			USActionComponent *ActionComp = Cast<USActionComponent>(NewBot->GetComponentByClass(USActionComponent::StaticClass()));
			if (ActionComp) {
				for (TSubclassOf<USAction> ActionClass : MonsterData->Actions) {
					ActionComp->AddAction(NewBot, ActionClass);
				}
			}
		}
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

void ASGameModeBase::WriteSaveGame() {
	// Iterate all player states since we don't have IDs.
	for (int32 i = 0; i < GameState->PlayerArray.Num(); ++i) {
		ASPlayerState *PS = Cast<ASPlayerState>(GameState->PlayerArray[i]);
		if (PS) {
			PS->SavePlayerState(CurrentSaveGame);
			break; // Single player only at this point.
		}
	}

	CurrentSaveGame->SavedActors.Empty();

	// Iterate the entire world of actors.
	for (FActorIterator It(GetWorld()); It; ++It) {
		AActor *Actor = *It;

		// Only interested in actors that can be interacted.
		if (!Actor->Implements<USGameplayInterface>()) {
			continue;
		}

		FActorSaveData ActorData;
		ActorData.ActorName = Actor->GetName();
		ActorData.Transform = Actor->GetTransform();

		// Pass the array to fill, with data from Actor.
		FMemoryWriter MemWriter(ActorData.ByteData);
		FObjectAndNameAsStringProxyArchive Ar(MemWriter, true);
		// Find only variable with UPROPERTY(SaveGame).
		Ar.ArIsSaveGame = true;
		// Convert Actor's variables with UPROPERTY(SaveGame) macro into binary array.
		// After calling Serialize(), ActorData.ByteData is filled.
		Actor->Serialize(Ar);

		CurrentSaveGame->SavedActors.Add(ActorData);
	}
	
	UGameplayStatics::SaveGameToSlot(CurrentSaveGame, SlotName, 0);
}

void ASGameModeBase::LoadSaveGame() {
	if (UGameplayStatics::DoesSaveGameExist(SlotName, 0)) {
		CurrentSaveGame = Cast<USSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));
		if (CurrentSaveGame == nullptr) {
			UE_LOG(LogTemp, Warning, TEXT("Failed to load SaveGame Data."));
			return;
		}
		UE_LOG(LogTemp, Log, TEXT("Loaded SaveGame Data."));

		for (FActorIterator It(GetWorld()); It; ++It) {
			AActor *Actor = *It;
			
			// Only interested in actors that can be interacted.
			if (!Actor->Implements<USGameplayInterface>()) {
				continue;
			}
			
			for (FActorSaveData ActorData : CurrentSaveGame->SavedActors) {
				if (ActorData.ActorName == Actor->GetName()) {
					Actor->SetActorTransform(ActorData.Transform);

					FMemoryReader MemReader(ActorData.ByteData);
					FObjectAndNameAsStringProxyArchive Ar(MemReader, true);
					Ar.ArIsSaveGame = true;
					// Convert binary array back into actor's variables.
					Actor->Serialize(Ar);

					ISGameplayInterface::Execute_OnActorLoaded(Actor);
					
					break;
				}
			}
		}
	} else {
		CurrentSaveGame = Cast<USSaveGame>(UGameplayStatics::CreateSaveGameObject(USSaveGame::StaticClass()));
		UE_LOG(LogTemp, Log, TEXT("Created New SaveGame Data."));
	}
}

void ASGameModeBase::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) {
	/* Calling before Super::, so we set variables before BeginPlayState is called in PlayerController (which is where we instantiate UI). */
	ASPlayerState *PS = NewPlayer->GetPlayerState<ASPlayerState>();
	if (PS) {
		PS->LoadPlayerState(CurrentSaveGame);
	}

	// It will call BeginPlayState in PlayerController.
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);
}
