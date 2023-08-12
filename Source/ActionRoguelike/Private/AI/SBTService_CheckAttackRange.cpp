// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTService_CheckAttackRange.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

USBTService_CheckAttackRange::USBTService_CheckAttackRange() {
	AttackRange = 1000.0f;
}

void USBTService_CheckAttackRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) {
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// Check distance between AI pawn and target actor.
	UBlackboardComponent *BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (ensure(BlackboardComp)) {
		AActor *TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject("TargetActor"));
		if (!IsValid(TargetActor)) {
			return;
		}
		AAIController *AIController = OwnerComp.GetAIOwner();
		if (ensure(AIController)) {
			APawn* AIPawn = AIController->GetPawn();
			if (ensure(AIPawn)) {
				float Distance = FVector::Distance(TargetActor->GetActorLocation(), AIPawn->GetActorLocation());
				bool bWithinRange = Distance < AttackRange;

				bool bCanSee = false;
				if (bWithinRange) {
					bCanSee = AIController->LineOfSightTo(TargetActor);
				}
				
				BlackboardComp->SetValueAsBool(AttackRangeKey.SelectedKeyName, bWithinRange && bCanSee);
			}
		}
	}
}

