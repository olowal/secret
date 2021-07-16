#pragma once

#include "SecGameplayAction.generated.h"

USTRUCT(BlueprintType)
struct FSecGameplayAction
{
	GENERATED_BODY()
public:
	const static FName LeftAxisRaw;
	const static FName RightAxisRaw;
	const static FName MovementDirection;
};
