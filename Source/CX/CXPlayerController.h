#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CXPlayerController.generated.h"

class UInputMappingContext;
class ACXPawn;
class UCXUI;

// 차량 플레이어 컨트롤러 클래스임
// 입력 매핑과 사용자 화면을 처리함
UCLASS(abstract)
class CX_API ACXPlayerController : public APlayerController
{
	GENERATED_BODY()
protected:
	friend class UChatUI;

	UFUNCTION(Server, Reliable)
	void CTSChatMessage(const FString& Message);

	UFUNCTION(Client, Reliable)
	void STCChatMessage(const FString& UserName, const FString& Message);

protected:

	// 플레이어 입력용 매핑 컨텍스트임
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* InputMappingContext;

	// 참이면 운전대 입력 매핑 컨텍스트를 등록함
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	bool bUseSteeringWheelControls = false;

	// 선택적으로 사용하는 운전대 입력 매핑 컨텍스트임
	// 기본 입력 매핑에 함께 추가되며 다른 입력을 막지 않음
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta=(EditCondition="bUseSteeringWheelControls"))
	UInputMappingContext* SteeringWheelInputMappingContext;

	// 조종 중인 차량 폰의 포인터임
	UPROPERTY(Replicated)
	TObjectPtr<ACXPawn> VehiclePawn;

	// 생성할 화면 위젯 클래스임
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = UI)
	TSubclassOf<UCXUI> VehicleUIClass;

	// 화면 위젯의 포인터임
	TObjectPtr<UCXUI> VehicleUI;
	UPROPERTY(EditDefaultsOnly, Category="UI") TSubclassOf<class UChatUI> ChatUIClass;
	UPROPERTY(Transient) TObjectPtr<class UChatUI> ChatUI;
	double LastChatTime = -1.;
	void OpenChat();

	

protected:

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

public:

	virtual void Tick(float Delta) override;

protected:

	virtual void OnPossess(APawn* InPawn) override;

};
