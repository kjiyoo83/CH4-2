
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CXGameMode.generated.h"

UCLASS(MinimalAPI)
class ACXGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ACXGameMode();
	virtual void InitGameState() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void EndPlay(const EEndPlayReason::Type Reason) override;
	UPROPERTY(EditDefaultsOnly, Category="Match", meta=(ClampMin="1")) int32 WaitingDuration = 10;
	UPROPERTY(EditDefaultsOnly, Category="Match", meta=(ClampMin="1")) int32 PlayingDuration = 120;

protected:
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

private:
	void AdvanceMatch();
	void FinishMatch();
	FTimerHandle MatchTimer;
	bool IsPlayerStartAvailable(const APlayerStart* PlayerStart) const;

private:
	UPROPERTY(EditDefaultsOnly)
	FVector VehicleSpawnCheckHalfExtent = FVector(250.0, 120.0, 80.0);

	UPROPERTY(EditDefaultsOnly)
	FVector VehicleSpawnCheckOffset = FVector(0.0, 0.0, 80.0);

};



