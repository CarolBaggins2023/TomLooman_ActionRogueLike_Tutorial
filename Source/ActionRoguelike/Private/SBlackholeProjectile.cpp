// Fill out your copyright notice in the Description page of Project Settings.


#include "SBlackholeProjectile.h"

#include "Components/SphereComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"

ASBlackholeProjectile::ASBlackholeProjectile() {
    SphereComp->SetCollisionObjectType(ECC_WorldDynamic);
    SphereComp->SetCollisionResponseToAllChannels(ECR_Overlap);

    ForceComp = CreateDefaultSubobject<URadialForceComponent>("ForceComp");
    ForceComp->RemoveObjectTypeToAffect(TEnumAsByte<EObjectTypeQuery>(ECC_Pawn));
    ForceComp->SetupAttachment(RootComponent);

    DestroyDelay = 5.0f;
}

void ASBlackholeProjectile::BeginPlay() {
    Super::BeginPlay();
    
    FTimerHandle TimerHandle_SelfDestroy;
    GetWorldTimerManager().SetTimer(TimerHandle_SelfDestroy, this, &ASBlackholeProjectile::SelfDestroy, DestroyDelay);
}

void ASBlackholeProjectile::SelfDestroy() {
    Destroy();
}
