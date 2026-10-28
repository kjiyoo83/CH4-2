
#include "CXGameMode.h"
#include "CXPlayerController.h"
#include "EngineUtils.h"
#include "Engine/World.h"
#include "GameFramework/PlayerStart.h"
#include "CXGameState.h"
#include "CXPlayerState.h"
#include "TimerManager.h"

ACXGameMode::ACXGameMode()
{
	PlayerControllerClass = ACXPlayerController::StaticClass();
	PlayerStateClass = ACXPlayerState::StaticClass();
	GameStateClass = ACXGameState::StaticClass();
}

void ACXGameMode::InitGameState()
{
 Super::InitGameState();
 if (ACXGameState* GS = GetGameState<ACXGameState>())
 {
  GS->MatchPhase = ECXMatchPhase::Waiting;
  GS->RemainingTime = FMath::Max(1, WaitingDuration);
  GS->Winners.Reset();
 }
}
void ACXGameMode::PostLogin(APlayerController* NewPlayer)
{
 Super::PostLogin(NewPlayer);
 const ACXGameState* GS = GetGameState<ACXGameState>();
 if (GS && GS->MatchPhase == ECXMatchPhase::Waiting && !GetWorldTimerManager().IsTimerActive(MatchTimer))
  GetWorldTimerManager().SetTimer(MatchTimer, this, &ThisClass::AdvanceMatch, 1.f, true);
}
void ACXGameMode::AdvanceMatch()
{
 ACXGameState* GS = GetGameState<ACXGameState>();
 if (!GS) return;
 GS->RemainingTime = FMath::Max(0, GS->RemainingTime - 1);
 if (GS->RemainingTime == 0)
 {
  if (GS->MatchPhase == ECXMatchPhase::Waiting)
  {
   GS->MatchPhase = ECXMatchPhase::Playing;
   GS->RemainingTime = FMath::Max(1, PlayingDuration);
  }
  else if (GS->MatchPhase == ECXMatchPhase::Playing) FinishMatch();
 }
 GS->ForceNetUpdate();
}
void ACXGameMode::FinishMatch()
{
 ACXGameState* GS = GetGameState<ACXGameState>();
 if (!GS) return;
 GS->Winners.Reset();
 int32 Best = -1;
 for (APlayerState* State : GS->PlayerArray)
 {
  ACXPlayerState* PS = Cast<ACXPlayerState>(State);
  if (!PS || PS->IsInactive()) continue;
  if (PS->GetCoins() > Best) { Best = PS->GetCoins(); GS->Winners.Reset(); }
  if (PS->GetCoins() == Best) GS->Winners.Add(PS);
 }
 GS->MatchPhase = ECXMatchPhase::Finished;
 GetWorldTimerManager().ClearTimer(MatchTimer);
}
void ACXGameMode::EndPlay(const EEndPlayReason::Type Reason)
{
 GetWorldTimerManager().ClearTimer(MatchTimer);
 Super::EndPlay(Reason);
}

AActor* ACXGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	TArray<APlayerStart*> AvailablePlayerStarts;

	for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
	{
		APlayerStart* PlayerStart = *It;

		if (IsPlayerStartAvailable(PlayerStart))
		{
			AvailablePlayerStarts.Add(PlayerStart);
		}
	}

	if (AvailablePlayerStarts.IsEmpty()) return Super::ChoosePlayerStart_Implementation(Player);

	const int32 RandomIndex = FMath::RandHelper(AvailablePlayerStarts.Num());
	return Cast<AActor>(AvailablePlayerStarts[RandomIndex]);
}

bool ACXGameMode::IsPlayerStartAvailable(const APlayerStart* PlayerStart) const
{
	if (IsValid(PlayerStart) == false || IsValid(GetWorld()) == false)
	{
		return false;
	}

	const FTransform PlayerStartTransform = PlayerStart->GetActorTransform();
	const FVector CheckLocation = PlayerStartTransform.TransformPosition(VehicleSpawnCheckOffset);
	const FQuat CheckRotation = PlayerStartTransform.GetRotation();
	const FCollisionShape CheckShape = FCollisionShape::MakeBox(VehicleSpawnCheckHalfExtent);
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Vehicle);

	const FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(VehiclePlayerStartCheck), false);

	const bool bVehicleExists =
		GetWorld()->OverlapAnyTestByObjectType(
			CheckLocation,
			CheckRotation,
			ObjectQueryParams,
			CheckShape,
			QueryParams
		);

	return !bVehicleExists;
}
