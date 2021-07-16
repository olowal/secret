#pragma once

#include "GameFramework/Actor.h"
#include "SecCombatArea.generated.h"

UCLASS(abstract)
class SECRET_API ASecCombatArea : public AActor
{
	GENERATED_BODY()
public:
	ASecCombatArea(const FObjectInitializer& ObjectInitializer);

	bool ValidateLocation(FVector& LocationToValidate) const;

	UFUNCTION(BlueprintNativeEvent, Category = CombatArea, meta = (DisplayName = "Validate Location"))
	bool BP_ValidateLocation(FVector& LocationToValidate) const;
};
