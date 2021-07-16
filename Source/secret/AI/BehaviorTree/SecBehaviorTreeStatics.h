#pragma once

#include "SecBehaviorTreeStatics.generated.h"

UENUM(BlueprintType)
enum class ESecBTEntryType : uint8
{
	// Enter a fixed value
	Value UMETA(DisplayName = "Value"),
	// Use a BBEntry
	BBEntry UMETA(DisplayName = "BBEntry"),
	Attribute UMETA(DisplayName = "Attribute"),
	Self UMETA(DisplayName = "Self")
};

UENUM(BlueprintType)
enum class ESecBTDistanceComparisonEntry : uint8
{
	Greater	UMETA(DisplayName = "Is Distance Greater Than"),
	Lower	UMETA(DisplayName = "Is Distance Lower Than")
};
