#include "CXPickup.h"
#include "CXPawn.h"
#include "CXPlayerState.h"
#include "CXGameState.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundBase.h"
#include "UObject/ConstructorHelpers.h"
ACXPickup::ACXPickup()
{
 bReplicates = true; bAlwaysRelevant = true; SetNetUpdateFrequency(1.f);
 Trigger = CreateDefaultSubobject<USphereComponent>(TEXT("Trigger")); SetRootComponent(Trigger);
 Trigger->SetSphereRadius(100.f); Trigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
 Trigger->SetCollisionResponseToAllChannels(ECR_Ignore); Trigger->SetCollisionResponseToChannel(ECC_Vehicle, ECR_Overlap); Trigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
 Trigger->SetGenerateOverlapEvents(true);
 Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh")); Mesh->SetupAttachment(Trigger); Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); Mesh->SetRelativeScale3D(FVector(.65f));
 static ConstructorHelpers::FObjectFinder<UStaticMesh> Shape(TEXT("/Engine/BasicShapes/Sphere.Sphere")); Mesh->SetStaticMesh(Shape.Object);
 static ConstructorHelpers::FObjectFinder<UParticleSystem> Effect(TEXT("/Game/StarterContent/Particles/P_Explosion.P_Explosion")); PickupEffect = Effect.Object;
 static ConstructorHelpers::FObjectFinder<USoundBase> Sound(TEXT("/Game/StarterContent/Audio/Explosion_Cue.Explosion_Cue")); PickupSound = Sound.Object;
}
ACXCoin::ACXCoin()
{
 bSpeedBuff = false;
 static ConstructorHelpers::FObjectFinder<UMaterialInterface> Material(TEXT("/Game/StarterContent/Materials/M_Metal_Gold.M_Metal_Gold")); Mesh->SetMaterial(0, Material.Object);
 Mesh->SetRelativeScale3D(FVector(.65f,.65f,.18f));
}
ACXSpeedBuff::ACXSpeedBuff()
{
 bSpeedBuff = true;
 static ConstructorHelpers::FObjectFinder<UStaticMesh> Shape(TEXT("/Engine/BasicShapes/Cube.Cube")); Mesh->SetStaticMesh(Shape.Object);
 static ConstructorHelpers::FObjectFinder<UMaterialInterface> Material(TEXT("/Game/LevelPrototyping/Materials/MI_Solid_Blue.MI_Solid_Blue")); Mesh->SetMaterial(0, Material.Object);
}
void ACXPickup::BeginPlay()
{
 Super::BeginPlay();
 if (HasAuthority()) Trigger->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnOverlap);
}
void ACXPickup::OnOverlap(UPrimitiveComponent* Component, AActor* Other, UPrimitiveComponent* OtherComp, int32 BodyIndex, bool bSweep, const FHitResult& Hit)
{
 ACXPawn* Car = Cast<ACXPawn>(Other);
 ACXPlayerState* PS = Car ? Car->GetPlayerState<ACXPlayerState>() : nullptr;
 const ACXGameState* GS = GetWorld()->GetGameState<ACXGameState>();
 if (!HasAuthority() || bCollected || !PS || !GS || GS->MatchPhase != ECXMatchPhase::Playing) return;
 const bool bGranted = bSpeedBuff ? PS->AddBonusMaxSpeed(10.f) : PS->AddCoins(10);
 if (!bGranted) return;
 bCollected = true; Trigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
 MulticastCollected(GetActorLocation()); Destroy();
}
void ACXPickup::MulticastCollected_Implementation(FVector Location)
{
 if (GetNetMode() == NM_DedicatedServer) return;
 if (PickupEffect) UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PickupEffect, Location);
 if (PickupSound) UGameplayStatics::PlaySoundAtLocation(this, PickupSound, Location);
}
