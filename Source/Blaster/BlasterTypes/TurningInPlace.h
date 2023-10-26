#pragma once

UENUM(BlueprintType)
enum class ETurningInPlace : uint8
{
	// TIP = Turning In Place
	ETIP_Left UMETA(DisplayName = "Turning Left"),
	ETIP_Right UMETA(DisplayName = "Turning Right"),
	ETIP_NotTurning UMETA(DisplayName = "Not Turning"),

	ETIP_MAX UMETA(DisplayName = "DefaultMAX")
};
