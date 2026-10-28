#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CXPickup.generated.h"
UCLASS()
class CX_API ACXPickup : public AActor
{
 GENERATED_BODY()
public:
 ACXPickup();
 virtual void BeginPlay() override;
 UPROPERTY(VisibleAnywhere) TObjectPtr<class USphereComponent> Trigger;
 UPROPERTY(VisibleAnywhere) TObjectPtr<class UStaticMeshComponent> Mesh;
 UPROPERTY(EditDefaultsOnly) bool bSpeedBuff = false;
 UPROPERTY(EditDefaultsOnly) TObjectPtr<class UParticleSystem> PickupEffect;
 UPROPERTY(EditDefaultsOnly) TObjectPtr<class USoundBase> PickupSound;
private:
 bool bCollected = false;
 UFUNCTION() void OnOverlap(UPrimitiveComponent* Component, AActor* Other, UPrimitiveComponent* OtherComp, int32 BodyIndex, bool bSweep, const FHitResult& Hit);
 UFUNCTION(NetMulticast, Reliable) void MulticastCollected(FVector Location);
};
UCLASS()
class CX_API ACXCoin : public ACXPickup
{
 GENERATED_BODY()
public: ACXCoin();
};
UCLASS()
class CX_API ACXSpeedBuff : public ACXPickup
{
 GENERATED_BODY()
public: ACXSpeedBuff();
};
