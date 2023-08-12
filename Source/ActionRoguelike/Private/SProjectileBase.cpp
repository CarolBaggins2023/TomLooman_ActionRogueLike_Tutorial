// Fill out your copyright notice in the Description page of Project Settings.


#include "SProjectileBase.h"

#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
ASProjectileBase::ASProjectileBase()
{
 	SphereComp = CreateDefaultSubobject<USphereComponent>("SphereComp");
	// Method One for setting collision.
	/*this->SphereComp->SetCollisionObjectType(ECC_WorldDynamic);
	this->SphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	this->SphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);*/
	// Method Two for setting collision with a file.
	SphereComp->SetCollisionProfileName("Projectile");
	SphereComp->OnComponentHit.AddDynamic(this, &ASProjectileBase::OnActorHit);
	RootComponent = SphereComp;

	EffectComp = CreateDefaultSubobject<UParticleSystemComponent>("EffectComp");
	EffectComp->SetupAttachment(RootComponent);
	
	MoveComp = CreateDefaultSubobject<UProjectileMovementComponent>("MoveComp");
	MoveComp->InitialSpeed = 8000.0f;
	MoveComp->ProjectileGravityScale = 0.0f;
	MoveComp->bRotationFollowsVelocity = true;
	MoveComp->bInitialVelocityInLocalSpace = true;

	FlightSound = CreateDefaultSubobject<UAudioComponent>("FlightSound");
	FlightSound->SetupAttachment(RootComponent);

	ImpactSound = CreateDefaultSubobject<UAudioComponent>("ImpactSound");
}

void ASProjectileBase::OnActorHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit) {
	// Note that we don't call Explode_Implementation, because that is just how we implement Explode in C++,
	// we may still implement Explode in Blueprint.
	Explode();
}

void ASProjectileBase::Explode_Implementation() {
	if (ensure(IsValid(this))) {
		UGameplayStatics::SpawnEmitterAtLocation(this, ImpactVFX, GetActorLocation(), GetActorRotation());
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound->Sound, GetActorLocation());
		Destroy();
		
		APlayerController * PC = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());
		PC->ClientStartCameraShake(CameraShake);
	}
}

void ASProjectileBase::PostInitializeComponents() {
	Super::PostInitializeComponents();
}

// Called when the game starts or when spawned
void ASProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	SphereComp->IgnoreActorWhenMoving(GetInstigator(), true);

	FlightSound->Activate();
}

// Called every frame
void ASProjectileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

