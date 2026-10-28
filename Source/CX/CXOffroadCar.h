#pragma once

#include "CoreMinimal.h"
#include "CXPawn.h"
#include "CXOffroadCar.generated.h"

UCLASS(abstract)
class CX_API ACXOffroadCar : public ACXPawn
{
	GENERATED_BODY()
	
	// 차체 스태틱 메시임
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Meshes, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Chassis;

	// 왼쪽 앞 타이어 스태틱 메시임
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Meshes, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* TireFrontLeft;

	// 오른쪽 앞 타이어 스태틱 메시임
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Meshes, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* TireFrontRight;

	// 왼쪽 뒤 타이어 스태틱 메시임
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Meshes, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* TireRearLeft;

	// 오른쪽 뒤 타이어 스태틱 메시임
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Meshes, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* TireRearRight;

public:

	ACXOffroadCar();
};
