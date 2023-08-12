// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SAICharacter.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/PawnSensingComponent.h"

// Sets default values
ASAICharacter::ASAICharacter()
{
	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>("PawnSensingComp");

	AttributeComp = CreateDefaultSubobject<USAttributeComponent>("AttributeComp");

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	LowHealthThreshold = 0.3f;
}

bool ASAICharacter::IsAlive() {
	return AttributeComp->IsAlive();
}

bool ASAICharacter::IsLowHealth() {
	return AttributeComp->GetHealth() < AttributeComp->GetHealthMax() * LowHealthThreshold;
}

void ASAICharacter::OnPawnSeen(APawn* Pawn) {
	AAIController *AIController = Cast<AAIController>(GetController());
	if (AIController) {
		UBlackboardComponent *BlackboardComp = AIController->GetBlackboardComponent();
		
		BlackboardComp->SetValueAsObject("TargetActor", Pawn);
	}
}

void ASAICharacter::PostInitializeComponents() {
	Super::PostInitializeComponents();

	PawnSensingComp->OnSeePawn.AddDynamic(this, &ASAICharacter::OnPawnSeen);

	AttributeComp->OnHealthChanged.AddDynamic(this, &ASAICharacter::OnHealthChanged);
}

void ASAICharacter::OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth,
	float Delta) {
	if (Delta < 0.0f) {
		GetMesh()->SetScalarParameterValueOnMaterials("TimeToHit", GetWorld()->TimeSeconds);
	}
}

