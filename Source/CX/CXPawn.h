#pragma once

#include "CoreMinimal.h"
#include "WheeledVehiclePawn.h"
#include "CXPawn.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UInputAction;
class UChaosWheeledVehicleMovementComponent;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateVehicle, Log, All);

// 차량 폰 클래스임
// 모든 차량의 공통 기능을 처리함
// 입력 처리와 카메라 관리를 포함함
// 차량별 설정은 하위 클래스에서 처리함
UCLASS(abstract)
class ACXPawn : public AWheeledVehiclePawn
{
	GENERATED_BODY()

	// 전방 카메라용 스프링 암임
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* FrontSpringArm;

	// 전방 카메라 컴포넌트임
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FrontCamera;

	// 후방 카메라용 스프링 암임
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* BackSpringArm;

	// 후방 카메라 컴포넌트임
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* BackCamera;

	// 카오스 차량 이동 컴포넌트로 변환한 포인터임
	TObjectPtr<UChaosWheeledVehicleMovementComponent> ChaosVehicleMovement;

protected:

	// 조향 입력 액션임
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* SteeringAction;

	// 가속 입력 액션임
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* ThrottleAction;

	// 제동 입력 액션임
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* BrakeAction;

	// 핸드브레이크 입력 액션임
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* HandbrakeAction;

	// 둘러보기 입력 액션임
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* LookAroundAction;

	// 카메라 전환 입력 액션임
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* ToggleCameraAction;

	// 차량 초기화 입력 액션임
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* ResetVehicleAction;

	// 전방 카메라 활성 여부를 저장함
	bool bFrontCameraActive = false;

public:
	ACXPawn();
	void StopDrivingForChat();
	virtual float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
protected:
	UPROPERTY(EditDefaultsOnly, Category="Combat") TObjectPtr<UInputAction> AttackAction;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat") TObjectPtr<class USceneComponent> AttackOrigin;
	UPROPERTY(EditDefaultsOnly, Category="Combat", meta=(ClampMin="1")) float AttackRange = 800.f;
	UPROPERTY(EditDefaultsOnly, Category="Combat", meta=(ClampMin="0.1")) float AttackCooldown = 0.5f;
	UPROPERTY(EditDefaultsOnly, Category="Combat") TObjectPtr<class UParticleSystem> HitEffect;
	UPROPERTY(EditDefaultsOnly, Category="Vehicle", meta=(ClampMin="1")) float BaseMaxSpeed = 120.f;
	void Attack();
	UFUNCTION(Server, Reliable) void ServerAttack();
	UFUNCTION(NetMulticast, Unreliable) void MulticastAttackEffect(FVector Start, FVector End, bool bHit);
	double LastAttackTime = -1.;
public:

	// 폰 인터페이스 시작임

	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

	// 폰 인터페이스 끝임

	// 액터 인터페이스 시작임

	virtual void Tick(float Delta) override;

	// 액터 인터페이스 끝임

protected:

	// 조향 입력을 처리함
	void Steering(const FInputActionValue& Value);

	// 가속 입력을 처리함
	void Throttle(const FInputActionValue& Value);

	// 제동 입력을 처리함
	void Brake(const FInputActionValue& Value);

	// 제동 시작과 해제 입력을 처리함
	void StartBrake(const FInputActionValue& Value);
	void StopBrake(const FInputActionValue& Value);

	// 핸드브레이크 시작과 해제 입력을 처리함
	void StartHandbrake(const FInputActionValue& Value);
	void StopHandbrake(const FInputActionValue& Value);

	// 둘러보기 입력을 처리함
	void LookAround(const FInputActionValue& Value);

	// 카메라 전환 입력을 처리함
	void ToggleCamera(const FInputActionValue& Value);

	// 차량 초기화 입력을 처리함
	void ResetVehicle(const FInputActionValue& Value);

	// 제동등을 켜거나 끌 때 호출됨
	UFUNCTION(BlueprintImplementableEvent, Category="Vehicle")
	void BrakeLights(bool bBraking);

public:
	// 전방 스프링 암을 반환함
	FORCEINLINE USpringArmComponent* GetFrontSpringArm() const { return FrontSpringArm; }
	// 전방 카메라를 반환함
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FrontCamera; }
	// 후방 스프링 암을 반환함
	FORCEINLINE USpringArmComponent* GetBackSpringArm() const { return BackSpringArm; }
	// 후방 카메라를 반환함
	FORCEINLINE UCameraComponent* GetBackCamera() const { return BackCamera; }
	// 변환된 카오스 차량 이동 컴포넌트를 반환함
	FORCEINLINE const TObjectPtr<UChaosWheeledVehicleMovementComponent>& GetChaosVehicleMovement() const { return ChaosVehicleMovement; }
};
