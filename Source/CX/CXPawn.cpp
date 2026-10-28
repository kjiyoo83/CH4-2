#include "CXPawn.h"
#include "CXWheelFront.h"
#include "CXWheelRear.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "ChaosWheeledVehicleMovementComponent.h"
#include "CXPlayerState.h"
#include "CXGameState.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "UObject/ConstructorHelpers.h"

#define LOCTEXT_NAMESPACE "VehiclePawn"

DEFINE_LOG_CATEGORY(LogTemplateVehicle);

ACXPawn::ACXPawn()
{
	bReplicates = true;
	SetReplicateMovement(true);
	AttackOrigin = CreateDefaultSubobject<USceneComponent>(TEXT("AttackOrigin"));
	AttackOrigin->SetupAttachment(GetMesh());
	AttackOrigin->SetRelativeLocation(FVector(220,0,40));
	static ConstructorHelpers::FObjectFinder<UParticleSystem> Impact(TEXT("/Game/StarterContent/Particles/P_Explosion.P_Explosion"));
	HitEffect = Impact.Object;
	// 전방 카메라 붐을 생성함
	FrontSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Front Spring Arm"));
	FrontSpringArm->SetupAttachment(GetMesh());
	FrontSpringArm->TargetArmLength = 0.0f;
	FrontSpringArm->bDoCollisionTest = false;
	FrontSpringArm->bEnableCameraRotationLag = true;
	FrontSpringArm->CameraRotationLagSpeed = 15.0f;
	FrontSpringArm->SetRelativeLocation(FVector(30.0f, 0.0f, 120.0f));

	FrontCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Front Camera"));
	FrontCamera->SetupAttachment(FrontSpringArm);
	FrontCamera->bAutoActivate = false;

	// 후방 카메라 붐을 생성함
	BackSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Back Spring Arm"));
	BackSpringArm->SetupAttachment(GetMesh());
	BackSpringArm->TargetArmLength = 650.0f;
	BackSpringArm->SocketOffset.Z = 150.0f;
	BackSpringArm->bDoCollisionTest = false;
	BackSpringArm->bInheritPitch = false;
	BackSpringArm->bInheritRoll = false;
	BackSpringArm->bEnableCameraRotationLag = true;
	BackSpringArm->CameraRotationLagSpeed = 2.0f;
	BackSpringArm->CameraLagMaxDistance = 50.0f;

	BackCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Back Camera"));
	BackCamera->SetupAttachment(BackSpringArm);

	// 차량 메시를 설정함
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionProfileName(FName("Vehicle"));
	GetMesh()->SetGenerateOverlapEvents(true);

	// 카오스 차량 이동 컴포넌트를 가져옴
	ChaosVehicleMovement = CastChecked<UChaosWheeledVehicleMovementComponent>(GetVehicleMovement());

}

void ACXPawn::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (AttackAction)
	{
	 if (auto* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent)) EIC->BindAction(AttackAction, ETriggerEvent::Started, this, &ThisClass::Attack);
	}
	else PlayerInputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &ThisClass::Attack);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// 조향 입력을 연결함 
		EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Triggered, this, &ACXPawn::Steering);
		EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Completed, this, &ACXPawn::Steering);

		// 가속 입력을 연결함 
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Triggered, this, &ACXPawn::Throttle);
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Completed, this, &ACXPawn::Throttle);

		// 제동 입력을 연결함 
		EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Triggered, this, &ACXPawn::Brake);
		EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Started, this, &ACXPawn::StartBrake);
		EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Completed, this, &ACXPawn::StopBrake);

		// 핸드브레이크 입력을 연결함 
		EnhancedInputComponent->BindAction(HandbrakeAction, ETriggerEvent::Started, this, &ACXPawn::StartHandbrake);
		EnhancedInputComponent->BindAction(HandbrakeAction, ETriggerEvent::Completed, this, &ACXPawn::StopHandbrake);

		// 둘러보기 입력을 연결함 
		EnhancedInputComponent->BindAction(LookAroundAction, ETriggerEvent::Triggered, this, &ACXPawn::LookAround);

		// 카메라 전환 입력을 연결함 
		EnhancedInputComponent->BindAction(ToggleCameraAction, ETriggerEvent::Triggered, this, &ACXPawn::ToggleCamera);

		// 차량 초기화 입력을 연결함 
		EnhancedInputComponent->BindAction(ResetVehicleAction, ETriggerEvent::Triggered, this, &ACXPawn::ResetVehicle);
	}
	else
	{
		UE_LOG(LogTemplateVehicle, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ACXPawn::Tick(float Delta)
{
	Super::Tick(Delta);
	if (HasAuthority() || IsLocallyControlled())
	{
	 const ACXPlayerState* PS = GetPlayerState<ACXPlayerState>();
	 const float Limit = (BaseMaxSpeed + (PS ? PS->GetBonusMaxSpeed() : 0.f)) / 0.036f;
	 FVector Velocity = GetMesh()->GetPhysicsLinearVelocity();
	 const float Horizontal = Velocity.Size2D();
	 if (Horizontal > Limit) { Velocity.X *= Limit / Horizontal; Velocity.Y *= Limit / Horizontal; GetMesh()->SetPhysicsLinearVelocity(Velocity); }
	}

	// 차량이 공중에 있으면 각도 감쇠를 추가함
	bool bMovingOnGround = ChaosVehicleMovement->IsMovingOnGround();
	GetMesh()->SetAngularDamping(bMovingOnGround ? 0.0f : 3.0f);

	// 카메라 수평 방향을 정면으로 맞춤
	float CameraYaw = BackSpringArm->GetRelativeRotation().Yaw;
	CameraYaw = FMath::FInterpTo(CameraYaw, 0.0f, Delta, 1.0f);

	BackSpringArm->SetRelativeRotation(FRotator(0.0f, CameraYaw, 0.0f));
}

void ACXPawn::Steering(const FInputActionValue& Value)
{
	// 조향 입력값을 가져옴
	float SteeringValue = Value.Get<float>();

	// 입력값을 적용함
	ChaosVehicleMovement->SetSteeringInput(SteeringValue);
}

void ACXPawn::Throttle(const FInputActionValue& Value)
{
	// 가속 입력값을 가져옴
	float ThrottleValue = Value.Get<float>();

	// 입력값을 적용함
	ChaosVehicleMovement->SetThrottleInput(ThrottleValue);
}

void ACXPawn::Brake(const FInputActionValue& Value)
{
	// 제동 입력값을 가져옴
	float BreakValue = Value.Get<float>();

	// 입력값을 적용함
	ChaosVehicleMovement->SetBrakeInput(BreakValue);
}

void ACXPawn::StartBrake(const FInputActionValue& Value)
{
	// 제동등 블루프린트 이벤트를 호출함
	BrakeLights(true);
}

void ACXPawn::StopBrake(const FInputActionValue& Value)
{
	// 제동등 블루프린트 이벤트를 호출함
	BrakeLights(false);

	// 제동 입력값을 0으로 초기화함
	ChaosVehicleMovement->SetBrakeInput(0.0f);
}

void ACXPawn::StartHandbrake(const FInputActionValue& Value)
{
	// 입력값을 적용함
	ChaosVehicleMovement->SetHandbrakeInput(true);

	// 제동등 블루프린트 이벤트를 호출함
	BrakeLights(true);
}

void ACXPawn::StopHandbrake(const FInputActionValue& Value)
{
	// 입력값을 적용함
	ChaosVehicleMovement->SetHandbrakeInput(false);

	// 제동등 블루프린트 이벤트를 호출함
	BrakeLights(false);
}

void ACXPawn::LookAround(const FInputActionValue& Value)
{
	// 입력의 수평 회전값을 가져옴 
	float LookValue = Value.Get<float>();

	// 입력값을 적용함
	BackSpringArm->AddLocalRotation(FRotator(0.0f, LookValue, 0.0f));
}

void ACXPawn::ToggleCamera(const FInputActionValue& Value)
{
	// 활성 카메라 상태를 전환함
	bFrontCameraActive = !bFrontCameraActive;

	FrontCamera->SetActive(bFrontCameraActive);
	BackCamera->SetActive(!bFrontCameraActive);
}

void ACXPawn::ResetVehicle(const FInputActionValue& Value)
{
	// 현재 위치보다 약간 위로 초기화함
	FVector ResetLocation = GetActorLocation() + FVector(0.0f, 0.0f, 50.0f);

	// 수평 회전만 유지하고 상하 회전과 기울기를 초기화함
	FRotator ResetRotation = GetActorRotation();
	ResetRotation.Pitch = 0.0f;
	ResetRotation.Roll = 0.0f;
	
	// 초기화 위치로 액터를 이동하고 물리 상태를 초기화함
	SetActorTransform(FTransform(ResetRotation, ResetLocation, FVector::OneVector), false, nullptr, ETeleportType::TeleportPhysics);

	GetMesh()->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
	GetMesh()->SetPhysicsLinearVelocity(FVector::ZeroVector);

	UE_LOG(LogTemplateVehicle, Error, TEXT("Reset Vehicle"));
}

#undef LOCTEXT_NAMESPACE

void ACXPawn::StopDrivingForChat()
{
 ChaosVehicleMovement->SetThrottleInput(0.f);
 ChaosVehicleMovement->SetSteeringInput(0.f);
 ChaosVehicleMovement->SetBrakeInput(0.f);
 ChaosVehicleMovement->SetHandbrakeInput(false);
}
void ACXPawn::Attack() { if (IsLocallyControlled()) ServerAttack(); }
void ACXPawn::ServerAttack_Implementation()
{
 const ACXGameState* GS = GetWorld()->GetGameState<ACXGameState>();
 const double Now = GetWorld()->GetTimeSeconds();
 if (!GS || GS->MatchPhase != ECXMatchPhase::Playing || Now - LastAttackTime < AttackCooldown) return;
 LastAttackTime = Now;
 const FVector Start = AttackOrigin->GetComponentLocation();
 const FVector End = Start + GetActorForwardVector() * AttackRange;
 FCollisionQueryParams Params(SCENE_QUERY_STAT(CXAttack), false, this);
 FHitResult Hit;
 GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);
 ACXPawn* Victim = Cast<ACXPawn>(Hit.GetActor());
 if (Victim && Victim != this) UGameplayStatics::ApplyDamage(Victim, 5.f, GetController(), this, nullptr);
 MulticastAttackEffect(Start, Hit.bBlockingHit ? Hit.ImpactPoint : End, Victim != nullptr);
}
float ACXPawn::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
 const ACXGameState* GS = GetWorld()->GetGameState<ACXGameState>();
 ACXPawn* Attacker = Cast<ACXPawn>(DamageCauser);
 ACXPlayerState* VictimPS = GetPlayerState<ACXPlayerState>();
 ACXPlayerState* AttackerPS = Attacker ? Attacker->GetPlayerState<ACXPlayerState>() : nullptr;
 if (!HasAuthority() || !GS || GS->MatchPhase != ECXMatchPhase::Playing || !AttackerPS || !VictimPS || AttackerPS == VictimPS || VictimPS->GetCoins() < 5 || AttackerPS->GetCoins() > MAX_int32 - 5) return 0.f;
 if (!VictimPS->RemoveCoins(5)) return 0.f;
 AttackerPS->AddCoins(5);
 Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
 return 5.f;
}
void ACXPawn::MulticastAttackEffect_Implementation(FVector Start, FVector End, bool bHit)
{
 DrawDebugLine(GetWorld(), Start, End, bHit ? FColor::Red : FColor::Green, false, 1.f, 0, 3.f);
 if (GetNetMode() != NM_DedicatedServer && bHit && HitEffect) UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitEffect, End);
}
