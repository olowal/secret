#pragma once

#include "SecActionState.generated.h"

UENUM(BlueprintType)
enum class ESecActionState : uint8
{
	ActiveForOneFrame,
	Active,
	Inactive
};
