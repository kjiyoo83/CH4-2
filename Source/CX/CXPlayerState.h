#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "CXPlayerState.generated.h"

UCLASS()
class CX_API ACXPlayerState : public APlayerState
{
 GENERATED_BODY()
public:
 virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
 UFUNCTION(BlueprintPure) int32 GetCoins() const { return Coins; }
 UFUNCTION(BlueprintPure) float GetBonusMaxSpeed() const { return BonusMaxSpeed; }
 UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) bool AddCoins(int32 Amount);
 UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) bool RemoveCoins(int32 Amount);
 UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) bool AddBonusMaxSpeed(float Amount);
private:
 UPROPERTY(Replicated, VisibleInstanceOnly, BlueprintReadOnly, meta=(AllowPrivateAccess="true")) int32 Coins = 0;
 // 속도 단위는 km/h이며 소유자에게만 복제됨. 물리는 서버의 권한 있는 값을 사용함
 UPROPERTY(Replicated, VisibleInstanceOnly, BlueprintReadOnly, meta=(AllowPrivateAccess="true")) float BonusMaxSpeed = 0.f;
};
