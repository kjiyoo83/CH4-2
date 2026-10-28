#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CXUI.generated.h"

UCLASS(abstract)
class CX_API UCXUI : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	// 속도 표시 단위를 시속 킬로미터 또는 마일로 설정함
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Vehicle)
	bool bIsMPH = false;

public:

	// 속도 표시를 갱신함
	void UpdateSpeed(float NewSpeed);

	// 기어 표시를 갱신함
	void UpdateGear(int32 NewGear);

protected:

	// 블루프린트에서 새 속도를 표시하도록 구현함
	UFUNCTION(BlueprintImplementableEvent, Category = Vehicle)
	void OnSpeedUpdate(float NewSpeed);

	// 블루프린트에서 새 기어를 표시하도록 구현함
	UFUNCTION(BlueprintImplementableEvent, Category = Vehicle)
	void OnGearUpdate(int32 NewGear);
};
