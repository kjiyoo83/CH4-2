#include "CXItemSpawner.h"
#include "CXPickup.h"
#include "TimerManager.h"
ACXItemSpawner::ACXItemSpawner()
{
	bReplicates = true;
 RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
 ItemClass = ACXCoin::StaticClass();
}
void ACXItemSpawner::BeginPlay() { Super::BeginPlay(); if (HasAuthority()) SpawnItem(); }
void ACXItemSpawner::SpawnItem()
{
 if (!HasAuthority() || bEnding || IsValid(SpawnedItem) || !ItemClass) return;
 FActorSpawnParameters Params; Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
 SpawnedItem = GetWorld()->SpawnActor<ACXPickup>(ItemClass, GetActorTransform(), Params);
 if (SpawnedItem) SpawnedItem->OnDestroyed.AddDynamic(this, &ThisClass::OnItemDestroyed);
 else GetWorldTimerManager().SetTimer(RespawnTimer, this, &ThisClass::SpawnItem, FMath::Max(.1f, RespawnDelay));
}
void ACXItemSpawner::OnItemDestroyed(AActor* Item)
{
 if (Item != SpawnedItem) return;
 SpawnedItem = nullptr;
 if (HasAuthority() && !bEnding) GetWorldTimerManager().SetTimer(RespawnTimer, this, &ThisClass::SpawnItem, FMath::Max(.1f, RespawnDelay));
}
void ACXItemSpawner::EndPlay(const EEndPlayReason::Type Reason)
{
 bEnding = true; GetWorldTimerManager().ClearTimer(RespawnTimer);
 if (IsValid(SpawnedItem)) { SpawnedItem->OnDestroyed.RemoveDynamic(this, &ThisClass::OnItemDestroyed); if (HasAuthority()) SpawnedItem->Destroy(); }
 Super::EndPlay(Reason);
}
