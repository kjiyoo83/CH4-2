#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/SpectatorPawn.h"
#include "GameFramework/WorldSettings.h"
#include "GameFramework/PlayerController.h"
#include "CXGameMode.h"
#include "CXGameState.h"
#include "CXPlayerState.h"
#include "CXPawn.h"
#include "CXPickup.h"
#include "CXItemSpawner.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Components/SphereComponent.h"
#include "InputAction.h"
#include "InputMappingContext.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCXAttackMappingTest, "CX.Gameplay.AttackInputMapping", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FCXAttackMappingTest::RunTest(const FString& Parameters)
{
 const UInputMappingContext* Context = LoadObject<UInputMappingContext>(nullptr, TEXT("/Game/VehicleTemplate/Input/IMC_Default.IMC_Default"));
 if (!TestNotNull(TEXT("Default driving input context exists"), Context)) return false;
 const FEnhancedActionKeyMapping* AttackMapping = Context->GetMappings().FindByPredicate([](const FEnhancedActionKeyMapping& Mapping)
 {
  return Mapping.Key == EKeys::LeftMouseButton && Mapping.Action && Mapping.Action->GetFName() == TEXT("IA_Attack");
 });
 return TestNotNull(TEXT("Saved context maps LeftMouseButton to IA_Attack"), AttackMapping);
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCXGameplayRulesTest, "CX.Gameplay.ServerRules", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FCXGameplayRulesTest::RunTest(const FString& Parameters)
{
 UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
 FWorldContext& Context = GEngine->CreateNewWorldContext(EWorldType::Game); Context.SetCurrentWorld(World);
 UGameInstance* GI = NewObject<UGameInstance>(GEngine); World->SetGameInstance(GI);
 World->GetWorldSettings()->DefaultGameMode = ACXGameMode::StaticClass();
 World->SetGameMode(FURL()); World->InitializeActorsForPlay(FURL());
 ACXGameMode* GM = World->GetAuthGameMode<ACXGameMode>();
 ACXGameState* GS = World->GetGameState<ACXGameState>();
 if (!TestNotNull(TEXT("GameMode initialized"), GM) || !TestNotNull(TEXT("GameState initialized"), GS)) return false;
 GM->DefaultPawnClass = ASpectatorPawn::StaticClass(); GM->PlayerControllerClass = APlayerController::StaticClass();
 World->SpawnActor<APlayerStart>();
 GM->WaitingDuration = 2; GM->PlayingDuration = 2; GS->RemainingTime = 2;
 World->BeginPlay();
 auto Advance = [World](float Seconds) { ++GFrameCounter; World->GetTimerManager().Tick(Seconds); };
 auto* PC1 = World->SpawnActor<APlayerController>();
 auto* PC2 = World->SpawnActor<APlayerController>();
 PC1->Player = NewObject<ULocalPlayer>(GEngine);
 PC2->Player = NewObject<ULocalPlayer>(GEngine);
 auto* PS1 = PC1->GetPlayerState<ACXPlayerState>();
 auto* PS2 = PC2->GetPlayerState<ACXPlayerState>();
 if (!TestNotNull(TEXT("Player 1 state"), PS1) || !TestNotNull(TEXT("Player 2 state"), PS2)) return false;
 TestFalse(TEXT("Reject negative coins"), PS1->AddCoins(-1));
 TestFalse(TEXT("Reject overdraft"), PS1->RemoveCoins(1));
 TestTrue(TEXT("Add coins"), PS1->AddCoins(10));
 TestTrue(TEXT("Bonus speed"), PS1->AddBonusMaxSpeed(10));
 TestEqual(TEXT("Bonus persisted"), PS1->GetBonusMaxSpeed(), 10.f);
 PS1->SetRole(ROLE_SimulatedProxy);
 TestFalse(TEXT("Client cannot add coins"), PS1->AddCoins(10));
 TestFalse(TEXT("Client cannot spend coins"), PS1->RemoveCoins(5));
 TestFalse(TEXT("Client cannot add speed"), PS1->AddBonusMaxSpeed(10));
 PS1->SetRole(ROLE_Authority);
 Advance(1.1f);
 TestEqual(TEXT("No countdown before login"), GS->RemainingTime, 2);
 GM->PostLogin(PC1); GM->PostLogin(PC2);
 Advance(1.1f); Advance(1.1f);
 TestEqual(TEXT("Waiting transitions to Playing"), GS->MatchPhase, ECXMatchPhase::Playing);
 UClass* CarClass = LoadClass<ACXPawn>(nullptr, TEXT("/Game/VehicleTemplate/Blueprints/SportsCar/BP_SportsCar_Pawn.BP_SportsCar_Pawn_C"));
 FActorSpawnParameters Spawn; Spawn.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
 auto* Car1 = World->SpawnActor<ACXPawn>(CarClass, FVector(0,0,200), FRotator::ZeroRotator, Spawn);
 auto* Car2 = World->SpawnActor<ACXPawn>(CarClass, FVector(700,0,200), FRotator(0,180,0), Spawn);
 PC1->Possess(Car1); PC2->Possess(Car2);
 Car2->ProcessEvent(Car2->FindFunctionChecked(TEXT("ServerAttack")), nullptr);
 TestEqual(TEXT("Victim loses 5"), PS1->GetCoins(), 5);
 TestEqual(TEXT("Attacker gains 5"), PS2->GetCoins(), 5);
 PS1->RemoveCoins(1);
 UGameplayStatics::ApplyDamage(Car1, 5.f, PC2, Car2, nullptr);
 TestEqual(TEXT("Victim below 5 unchanged"), PS1->GetCoins(), 4);
 TestEqual(TEXT("No coin creation below 5"), PS2->GetCoins(), 5);
 auto* Coin = World->SpawnActor<ACXCoin>(FVector(6000,0,200), FRotator::ZeroRotator);
 Coin->SetActorLocation(Car1->GetActorLocation());
 TestEqual(TEXT("Coin overlap awards 10"), PS1->GetCoins(), 14);
 TestTrue(TEXT("Collected coin destroyed"), Coin->IsActorBeingDestroyed());
 auto* Buff = World->SpawnActor<ACXSpeedBuff>(FVector(6000,0,200), FRotator::ZeroRotator);
 Buff->SetActorLocation(Car1->GetActorLocation());
 TestEqual(TEXT("Buff overlap awards 10 speed"), PS1->GetBonusMaxSpeed(), 20.f);
 TestTrue(TEXT("Collected buff destroyed"), Buff->IsActorBeingDestroyed());
 PS1->RemoveCoins(10);
 auto* Spawner = World->SpawnActor<ACXItemSpawner>(FVector(5000,0,200), FRotator::ZeroRotator);
 TestNotNull(TEXT("Spawner creates item"), Spawner->SpawnedItem.Get());
 if (Spawner->SpawnedItem)
 {
  TestEqual(TEXT("Pickup update frequency"), Spawner->SpawnedItem->GetNetUpdateFrequency(), 1.f);
  Spawner->SpawnedItem->Destroy();
  TestNull(TEXT("Destroyed item reference cleared"), Spawner->SpawnedItem.Get());
  Advance(1.f);
  TestNull(TEXT("No respawn before 2 seconds"), Spawner->SpawnedItem.Get());
  Advance(1.1f);
  TestNotNull(TEXT("Respawn after 2 seconds"), Spawner->SpawnedItem.Get());
 }
 // 경기 종료 후 공동 우승 확인용 짧은 경기를 다시 초기화함
 PS1->AddCoins(1);
 GS->MatchPhase = ECXMatchPhase::Waiting; GS->RemainingTime = 1;
 GM->PostLogin(PC1);
 Advance(1.1f); Advance(1.1f); Advance(1.1f);
 TestEqual(TEXT("Playing transitions to Finished"), GS->MatchPhase, ECXMatchPhase::Finished);
 TestEqual(TEXT("Both tied players win"), GS->Winners.Num(), 2);
 UGameplayStatics::ApplyDamage(Car1, 5.f, PC2, Car2, nullptr);
 TestEqual(TEXT("Finished match rejects damage"), PS1->GetCoins(), 5);
 World->EndPlay(EEndPlayReason::Quit);
 World->DestroyWorld(false); GEngine->DestroyWorldContext(World);
 return true;
}
#endif
