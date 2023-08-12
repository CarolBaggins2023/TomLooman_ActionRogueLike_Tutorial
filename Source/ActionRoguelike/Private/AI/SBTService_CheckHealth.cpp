// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTService_CheckHealth.h"

#include "AI/SAICharacter.h"
#include "AI/SAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

void USBTService_CheckHealth::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) {
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController *AIController = OwnerComp.GetAIOwner();
	if (ensure(AIController)) {
		ASAICharacter *AIPawn = Cast<ASAICharacter>(AIController->GetPawn());
		if (ensure(AIPawn)) {
			UBlackboardComponent *BlackboardComp = OwnerComp.GetBlackboardComponent();
			if (ensure(BlackboardComp)) {
				BlackboardComp->SetValueAsBool(LowHealthKey.SelectedKeyName, AIPawn->IsLowHealth());
			}
		}
	}
}
