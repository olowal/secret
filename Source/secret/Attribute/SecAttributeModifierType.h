#pragma once

#include "SecAttributeModifierType.generated.h"

UENUM(BlueprintType)
enum class ESecAttributeModifierType : uint8
{
	Add,
	Sub,
	Mul,
	Div,
	Override,
	Custom
};
