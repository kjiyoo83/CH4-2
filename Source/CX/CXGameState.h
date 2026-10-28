#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "CXGameState.generated.h"
class ACXPlayerState;
UENUM(BlueprintType)
enum class ECXMatchPhase : uint8 { Waiting, Playing, Finished };
UCLASS()
class CX_API ACXGameState : public AGameStateBase
{
 GENERATED_BODY()
public:
 virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
 UPROPERTY(Replicated, BlueprintReadOnly) ECXMatchPhase MatchPhase = ECXMatchPhase::Waiting;
 UPROPERTY(Replicated, BlueprintReadOnly) int32 RemainingTime = 0;
 UPROPERTY(Replicated, BlueprintReadOnly) TArray<TObjectPtr<ACXPlayerState>> Winners;
};
