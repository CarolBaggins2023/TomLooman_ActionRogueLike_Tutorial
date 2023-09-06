// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SAICharacter.h"

#include "AIController.h"
#include "BrainComponent.h"
#include "SActionComponent.h"
#include "SWorldUserWidget.h"
#include "AI/SAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/PawnSensingComponent.h"

// Sets default values
ASAICharacter::ASAICharacter()
{
	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>("PawnSensingComp");

	AttributeComp = CreateDefaultSubobject<USAttributeComponent>("AttributeComp");

	ActionComp = CreateDefaultSubobject<USActionComponent>("ActionComp");
	
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	LowHealthThreshold = 0.3f;

	TimeToHitParamName = "TimeToHit";

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);
}

bool ASAICharacter::IsAlive() {
	return USAttributeComponent::IsActorAlive(this);
}

bool ASAICharacter::IsLowHealth() {
	return AttributeComp->GetHealth() < AttributeComp->GetHealthMax() * LowHealthThreshold;
}

void ASAICharacter::PostInitializeComponents() {
	Super::PostInitializeComponents();

	PawnSensingComp->OnSeePawn.AddDynamic(this, &ASAICharacter::OnPawnSeen);

	AttributeComp->OnHealthChanged.AddDynamic(this, &ASAICharacter::OnHealthChanged);
}

void ASAICharacter::SetTargetActor(AActor* TargetActor) {
	AAIController *AIController = Cast<AAIController>(GetController());
	if (AIController) {
		AIController->GetBlackboardComponent()->SetValueAsObject("TargetActor", TargetActor);
	}

	if (PlayerSpottedWidget == nullptr && ensure(PlayerSpottedWidgetClass)) {
		PlayerSpottedWidget = CreateWidget<USWorldUserWidget>(GetWorld(), PlayerSpottedWidgetClass);
		if (ensure(PlayerSpottedWidget)) {
			PlayerSpottedWidget->AttachedActor = this;
			// Index which is higher than default (0) places itself on the top of any other widget.
			// Otherwise, it may end up behind the minion health bar.
			PlayerSpottedWidget->AddToViewport(10);
		}
	}
}

AActor* ASAICharacter::GetTargetActor() {
	AAIController *AIController = Cast<AAIController>(GetController());
	if (AIController) {
		UBlackboardComponent *BlackboardComp = AIController->GetBlackboardComponent();
		if (BlackboardComp) {
			return Cast<AActor>(BlackboardComp->GetValueAsObject("TargetActor"));
		}
	}
	return nullptr;
}

void ASAICharacter::OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta) {
	if (Delta < 0.0f) {

		if (InstigatorActor != this) {
			SetTargetActor(InstigatorActor);
		}

		if (ActiveHealthBar == nullptr) {
			ActiveHealthBar = CreateWidget<USWorldUserWidget>(GetWorld(), HealthBarWidgetClass);
			if (ActiveHealthBar) {
				ActiveHealthBar->AttachedActor = this;
				ActiveHealthBar->AddToViewport();
			}
		}
		
		GetMesh()->SetScalarParameterValueOnMaterials(TimeToHitParamName, GetWorld()->TimeSeconds);
	}
	
	if (!USAttributeComponent::IsActorAlive(this)) {
		ASAIController *AIController = Cast<ASAIController>(GetController());
		if (AIController) {
			AIController->GetBrainComponent()->StopLogic("Killed");
		}

		GetMesh()->SetAllBodiesSimulatePhysics(true);
		GetMesh()->SetCollisionProfileName("Ragdoll");
		
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetCharacterMovement()->DisableMovement();

		SetLifeSpan(5.0f);
	}
}

void ASAICharacter::OnPawnSeen(APawn* Pawn) {
	if (GetTargetActor() == Pawn) {
		return;
	}
	
	SetTargetActor(Pawn);
	// DrawDebugString(GetWorld(), Pawn->GetActorLocation(), TEXT("Player Spotted"), nullptr, FColor::White, 0.5f, true);

	MulticastPawnSeen();
}

void ASAICharacter::MulticastPawnSeen_Implementation() {
	if (PlayerSpottedWidget == nullptr && ensure(PlayerSpottedWidgetClass)) {
		PlayerSpottedWidget = CreateWidget<USWorldUserWidget>(GetWorld(), PlayerSpottedWidgetClass);
		if (ensure(PlayerSpottedWidget)) {
			PlayerSpottedWidget->AttachedActor = this;
			// Index which is higher than default (0) places itself on the top of any other widget.
			// Otherwise, it may end up behind the minion health bar.
			PlayerSpottedWidget->AddToViewport(10);
		}
	}
}
