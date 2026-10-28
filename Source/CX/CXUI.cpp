#include "CXUI.h"

void UCXUI::UpdateSpeed(float NewSpeed)
{
	// 속도를 시속 킬로미터 또는 마일로 변환함
	float FormattedSpeed = FMath::Abs(NewSpeed) * (bIsMPH ? 0.022f : 0.036f);

	// 블루프린트 처리 함수를 호출함
	OnSpeedUpdate(FormattedSpeed);
}

void UCXUI::UpdateGear(int32 NewGear)
{
	// 블루프린트 처리 함수를 호출함
	OnGearUpdate(NewGear);
}