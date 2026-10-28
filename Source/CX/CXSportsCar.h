// 저작권은 Epic Games, Inc.에 있으며 모든 권리를 보유함

#pragma once

#include "CoreMinimal.h"
#include "CXPawn.h"
#include "CXSportsCar.generated.h"

// 스포츠카를 구현함
UCLASS(abstract)
class CX_API ACXSportsCar : public ACXPawn
{
	GENERATED_BODY()
	
public:

	ACXSportsCar();
};
