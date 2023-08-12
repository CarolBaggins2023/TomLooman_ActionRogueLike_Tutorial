// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

void ASAIController::BeginPlay() {
	Super::BeginPlay();

	if (ensureMsgf(BehaviorTree, TEXT("BehaviorTree is NULL!"))) {
		RunBehaviorTree(BehaviorTree);
	}

	// Following code are replace by SACharacter::OnPawnSeen.
	// APawn *MyPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	// if (ensure(MyPawn)) {
	// 	GetBlackboardComponent()->SetValueAsVector("MoveToLocation", MyPawn->GetActorLocation());
	//
	// 	GetBlackboardComponent()->SetValueAsObject("TargetActor", MyPawn);
	// }
}
