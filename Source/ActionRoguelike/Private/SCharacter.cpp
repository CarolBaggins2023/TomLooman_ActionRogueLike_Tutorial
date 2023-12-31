// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionRoguelike/Public/SCharacter.h"

#include "SAttributeComponent.h"
#include "SDashProjectile.h"
#include "SInteractionComponent.h"
#include "SPlayerState.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Physics/PhysicsFiltering.h"

// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>("SpringArmComp");
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->bUsePawnControlRotation = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>("CameraComp");
	CameraComp->SetupAttachment(SpringArmComp);

	InteractionComp = CreateDefaultSubobject<USInteractionComponent>("InteractionComp");

	AttributeComp = CreateDefaultSubobject<USAttributeComponent>("AttributeComp");

	ActionComp = CreateDefaultSubobject<USActionComponent>("ActionComp");
	
	GetCharacterMovement()->bOrientRotationToMovement = true;
	this->bUseControllerRotationYaw = false;
}

void ASCharacter::PostInitializeComponents() {
	Super::PostInitializeComponents();

	AttributeComp->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);
}

bool ASCharacter::HealSelf(float Amount /* = 100.0f */) {
	return AttributeComp->ApplyHealthChange(this, Amount);
}

FVector ASCharacter::GetPawnViewLocation() const {
	return CameraComp->GetComponentLocation();
}

void ASCharacter::MoveForward(float val) {
	FRotator ControlRot = GetControlRotation();
	ControlRot.Pitch = 0.0f;
	ControlRot.Roll = 0.0f;
	
	AddMovementInput(ControlRot.Vector(), val);
}

void ASCharacter::MoveRight(float val) {
	FRotator ControlRot = GetControlRotation();
	ControlRot.Pitch = 0.0f;
	ControlRot.Roll = 0.0f;

	FVector RightVector = FRotationMatrix(ControlRot).GetScaledAxis(EAxis::Y);
	
	AddMovementInput(RightVector, val);
}

void ASCharacter::SprintStart() {
	ActionComp->StartActionByName(this, "Sprint");
}

void ASCharacter::SprintStop() {
	ActionComp->StopActionByName(this, "Sprint");
}

void ASCharacter::ParryStart() {
	ActionComp->StartActionByName(this, "Parry");
}

void ASCharacter::PrimaryAttack() {
	// They are replaced by action component.
	/*TurnToAttackDirection();
	PlayAnimMontage(AttackAnim);
	
	UGameplayStatics::SpawnEmitterAttached(MuzzleFlash, GetMesh(), FName(RightHandSocketName));

	GetWorldTimerManager().SetTimer(TimerHandle_PrimaryAttack, this,
		&ASCharacter::PrimaryAttack_TimeElapsed, AttackAnimDelay);*/

	ActionComp->StartActionByName(this, "PrimaryAttack");
}

void ASCharacter::DashAttack() {
	bool HasDashAttack = ActionComp->StartActionByName(this, "DashAttack");
	if (!HasDashAttack) {
		FString Msg = FString::Printf(TEXT("Don't have Action: DashAttack."));
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::White, Msg);
	}
}

void ASCharacter::BlackholeAttack() {
	ActionComp->StartActionByName(this, "BlackholeAttack");
}

void ASCharacter::PrimaryInteract() {
	if (InteractionComp) {
		InteractionComp->PrimaryInteract();
	}
}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
 
	PlayerInputComponent->BindAction("PrimaryAttack", IE_Pressed, this, &ASCharacter::PrimaryAttack);
	PlayerInputComponent->BindAction("DashAttack", IE_Pressed, this, &ASCharacter::DashAttack);
	PlayerInputComponent->BindAction("BlackholeAttack", IE_Pressed, this, &ASCharacter::BlackholeAttack);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);

	PlayerInputComponent->BindAction("PrimaryInteract", IE_Pressed, this, &ASCharacter::PrimaryInteract);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ASCharacter::SprintStart);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ASCharacter::SprintStop);

	PlayerInputComponent->BindAction("Parry", IE_Pressed, this, &ASCharacter::ParryStart);
}

void ASCharacter::OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth,
	float Delta) {
	if (Delta < 0.0f) {
		GetMesh()->SetScalarParameterValueOnMaterials("TimeToHit", GetWorld()->TimeSeconds);

		AttributeComp->ApplyRageChange(this, abs(Delta));
	}
	if (NewHealth <= 0.0f) {
		APlayerController *PC = Cast<APlayerController>(GetController());
		DisableInput(PC);
		
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		SetLifeSpan(5.0f);
	}
}

void ASCharacter::AddCredits(int32 CreditsAmount /* = 10000 */) {
	// Since ASCharacter already has a member named PlayerState, the local variable in this function can't be
	// named PlayerState again. (Local variable 'can' have the same name with member variable, but not in this function.)
	// Or when we call PlayerState->AddCredits(), it can not figure out which PlayerState are called.
	// In ASGameModeBase::OnActorKilled, we can have a local variable named PlayerState, because GameMode has
	// no member called PlayerState.
	ASPlayerState *PS = GetPlayerState<ASPlayerState>();
	if (PS) {
		PS->AddCredits(CreditsAmount);
	}
	
	FString Msg = FString::Printf(TEXT("Add credits: %d."), CreditsAmount);
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, Msg);
}

void ASCharacter::AddRage(float RageAmount /* = 100 */) {
	AttributeComp->ApplyRageChange(this, RageAmount);

	FString Msg = FString::Printf(TEXT("Add rage: %.0f."), RageAmount);
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, Msg);
}
