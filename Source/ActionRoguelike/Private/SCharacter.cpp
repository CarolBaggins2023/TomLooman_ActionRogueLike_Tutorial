// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionRoguelike/Public/SCharacter.h"

#include "SAttributeComponent.h"
#include "SDashProjectile.h"
#include "SInteractionComponent.h"
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

	MuzzleFlash = CreateDefaultSubobject<UParticleSystem>("MuzzleFlash");
	
	GetCharacterMovement()->bOrientRotationToMovement = true;
	this->bUseControllerRotationYaw = false;
	
	AttackAnimDelay = 0.2f;

	bIsAccelerating = false;

	RightHandSocketName = "Muzzle_01";
	
}

void ASCharacter::PostInitializeComponents() {
	Super::PostInitializeComponents();

	AttributeComp->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);
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

void ASCharacter::TurnToAttackDirection() {
	FRotator ControlRotator = GetControlRotation();
	ControlRotator.Pitch = GetActorRotation().Pitch;
	ControlRotator.Roll = GetActorRotation().Roll;
	SetActorRotation(ControlRotator, ETeleportType::None);
}

void ASCharacter::PrimaryAttack() {
	TurnToAttackDirection();
	PlayAnimMontage(AttackAnim);
	
	UGameplayStatics::SpawnEmitterAttached(MuzzleFlash, GetMesh(), FName(RightHandSocketName));

	GetWorldTimerManager().SetTimer(TimerHandle_PrimaryAttack, this,
		&ASCharacter::PrimaryAttack_TimeElapsed, AttackAnimDelay);
}

void ASCharacter::PrimaryAttack_TimeElapsed() {
	SpawnProjectile(PrimaryProjectileClass);
}

void ASCharacter::DashAttack() {
	TurnToAttackDirection();
	PlayAnimMontage(AttackAnim);

	GetWorldTimerManager().SetTimer(TimerHandle_Dash, this,
		&ASCharacter::DashAttack_TimeElapsed, AttackAnimDelay);
}

void ASCharacter::DashAttack_TimeElapsed() {
	SpawnProjectile(DashProjectileClass);
}

void ASCharacter::BlackholeAttack() {
	TurnToAttackDirection();
	PlayAnimMontage(AttackAnim);

	GetWorldTimerManager().SetTimer(TimerHandle_Backhole, this,
		&ASCharacter::BlackholeAttack_TimeElapsed, AttackAnimDelay);
}

void ASCharacter::BlackholeAttack_TimeElapsed() {
	SpawnProjectile(BlackholeProjectileClass);
}

void ASCharacter::SpawnProjectile(TSubclassOf<AActor> ClassToSpawn) {
	if (ensureAlways(ClassToSpawn)) {
		// Get Start and End vector to construct rotator.
		FVector RightHandLocation = GetMesh()->GetSocketLocation(FName(RightHandSocketName));
		FVector Start = RightHandLocation;
		// Get End vector is somewhat troublesome.

		// Perform line trace detection.
		FHitResult Hit;
		
		FVector TraceStart = CameraComp->GetComponentLocation();
		FVector TraceEnd = TraceStart + GetControlRotation().Vector() * 5000.0f;
		
		FCollisionObjectQueryParams ObjectQueryParams;
		ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
		ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
		ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);

		FCollisionShape Shape;
		Shape.SetSphere(20.0f);

		// Ignore the player.
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);
		
		bool bBlockingHit = GetWorld()->SweepSingleByObjectType(Hit, TraceStart, TraceEnd,
			FQuat::Identity, ObjectQueryParams, Shape, QueryParams);
		
		// Is the judgement redundant? Because Hit.GetActor()->GetActorLocation() and LineTraceEnd
		// are two points at the same line.
		// Answer: It makes sense. The key point here is that the start point is not at the same line with them.
		FVector End = bBlockingHit ? Hit.ImpactPoint : TraceEnd;

		// Get the transformation of spawned projectile.
		FRotator ProjectileRotation = FRotationMatrix::MakeFromX(End - Start).Rotator();
		FTransform SpawnTM = FTransform(ProjectileRotation, RightHandLocation);

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Instigator = this;
	
		GetWorld()->SpawnActor<AActor>(ClassToSpawn, SpawnTM, SpawnParams);
	} else {
		UE_LOG(LogTemp, Warning, TEXT("The ClassToSpawn is NULL."));
	}
}

void ASCharacter::PrimaryInteract() {
	if (InteractionComp) {
		InteractionComp->PrimaryInteract();
	}
}

void ASCharacter::Accelarate() {
	if (bIsAccelerating) {
		GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	} else {
		GetCharacterMovement()->MaxWalkSpeed = 1200.0f;
	}
	bIsAccelerating = !bIsAccelerating;
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
	PlayerInputComponent->BindAction("Accelerating", IE_Pressed, this, &ASCharacter::Accelarate);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);

	PlayerInputComponent->BindAction("PrimaryInteract", IE_Pressed, this, &ASCharacter::PrimaryInteract);
}

void ASCharacter::OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth,
	float Delta) {
	if (Delta < 0.0f) {
		GetMesh()->SetScalarParameterValueOnMaterials("TimeToHit", GetWorld()->TimeSeconds);
	}
	if (NewHealth <= 0.0f) {
		APlayerController *PC = Cast<APlayerController>(GetController());
		DisableInput(PC);
		
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}
