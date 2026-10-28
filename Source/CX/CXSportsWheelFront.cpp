// 저작권은 Epic Games, Inc.에 있으며 모든 권리를 보유함


#include "CXSportsWheelFront.h"

UCXSportsWheelFront::UCXSportsWheelFront()
{
	WheelRadius = 39.0f;
	WheelWidth = 35.0f;
	FrictionForceMultiplier = 3.0f;

	MaxBrakeTorque = 4500.0f;
	MaxHandBrakeTorque = 6000.0f;
}