// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTTask_HealthSelf.h"

#include "AI/SAICharacter.h"
#include "AI/SAIController.h"

EBTNodeResult::Type USBTTask_HealthSelf::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
	AAIController *AIController = OwnerComp.GetAIOwner();
	if (ensure(AIController)) {
		ASAICharacter *AIPawn = Cast<ASAICharacter>(AIController->GetPawn());
		if (ensure(AIPawn)) {
			USAttributeComponent *AttributeComp = Cast<USAttributeComponent>(AIPawn->GetComponentByClass(USAttributeComponent::StaticClass()));
			AttributeComp->ApplyHealthChange(AIPawn, AttributeComp->GetHealthMax());
			return AttributeComp->IsInjured() ? EBTNodeResult::Failed : EBTNodeResult::Succeeded;
		}
	}
	return EBTNodeResult::Failed;
}
