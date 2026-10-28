// 플레이어 컨트롤러를 구현함


#include "CXPlayerController.h"
#include "CXPawn.h"
#include "CXUI.h"
#include "EnhancedInputSubsystems.h"
#include "ChaosWheeledVehicleMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "UI/ChatUI.h"
#include "CXPlayerState.h"
#include "CXGameState.h"
#include "EngineUtils.h"

void ACXPlayerController::CTSChatMessage_Implementation(const FString& Message)
{
	FString Clean = Message.Left(256).TrimStartAndEnd();
	Clean.ReplaceInline(TEXT("\r"), TEXT(" ")); Clean.ReplaceInline(TEXT("\n"), TEXT(" "));
	const double Now = GetWorld()->GetTimeSeconds();
	if (Clean.IsEmpty() || Now - LastChatTime < 0.3) return;
	LastChatTime = Now;
	const FString Name = PlayerState ? PlayerState->GetPlayerName() : TEXT("Player");
	for (TActorIterator<ACXPlayerController> It(GetWorld()); It; ++It) It->STCChatMessage(Name, Clean);
}

void ACXPlayerController::STCChatMessage_Implementation(const FString& UserName, const FString& Message)
{
	if (ChatUI) ChatUI->OnNewChatMessage(UserName, Message);
}

void ACXPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController() == false)
	{
		return;
	}
	
	// 화면 위젯을 생성해 뷰포트에 추가함
	if (VehicleUIClass) VehicleUI = CreateWidget<UCXUI>(this, VehicleUIClass);
	if (VehicleUI) VehicleUI->AddToViewport();
	ChatUI = CreateWidget<UChatUI>(this, ChatUIClass ? ChatUIClass.Get() : UChatUI::StaticClass());
	if (ChatUI) ChatUI->AddToViewport(10);
}

void ACXPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindKey(EKeys::Enter, IE_Pressed, this, &ThisClass::OpenChat);
	
	// 향상된 입력 서브시스템을 가져옴
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		// 조작용 입력 매핑 컨텍스트를 추가함
		if (InputMappingContext) Subsystem->AddMappingContext(InputMappingContext, 0);

		// 필요하면 운전대 입력 컨텍스트를 추가함
		if (bUseSteeringWheelControls && SteeringWheelInputMappingContext)
		{
			Subsystem->AddMappingContext(SteeringWheelInputMappingContext, 1);
		}
	}
}

void ACXPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, VehiclePawn);
}

void ACXPlayerController::Tick(float Delta)
{
	Super::Tick(Delta);
	VehiclePawn = Cast<ACXPawn>(GetPawn());
	if (ChatUI)
	{
	 const ACXPlayerState* PS = GetPlayerState<ACXPlayerState>();
	 const ACXGameState* GS = GetWorld()->GetGameState<ACXGameState>();
	 if (PS && GS)
	 {
	  FString Phase = GS->MatchPhase == ECXMatchPhase::Waiting ? TEXT("Waiting") : GS->MatchPhase == ECXMatchPhase::Playing ? TEXT("Playing") : TEXT("Finished");
	  FString Result;
	  if (GS->MatchPhase == ECXMatchPhase::Finished) Result = GS->Winners.Contains(PS) ? TEXT(" | VICTORY") : TEXT(" | DEFEAT");
	  ChatUI->UpdateStatus(FString::Printf(TEXT("Coins: %d won | Bonus: +%.0f km/h\n%s | %d s%s"), PS->GetCoins(), PS->GetBonusMaxSpeed(), *Phase, GS->RemainingTime, *Result));
	 }
	}

	if (IsValid(VehiclePawn) && IsValid(VehicleUI))
	{
		VehicleUI->UpdateSpeed(VehiclePawn->GetChaosVehicleMovement()->GetForwardSpeed());
		VehicleUI->UpdateGear(VehiclePawn->GetChaosVehicleMovement()->GetCurrentGear());
	}
}

void ACXPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// 조종 중인 폰의 포인터를 가져옴
	VehiclePawn = Cast<ACXPawn>(InPawn);
}
void ACXPlayerController::OpenChat() { if (ChatUI) ChatUI->FocusChat(); }
