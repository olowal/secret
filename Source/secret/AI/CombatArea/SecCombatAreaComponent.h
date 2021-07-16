#pragma once

#include "Components/ActorComponent.h"
#include "SecCombatAreaComponent.generated.h"

class ASecCombatArea;

UCLASS(BlueprintType, meta = (BlueprintSpawnableComponent))
class SECRET_API USecCombatAreaComponent : public UActorComponent
{
	GENERATED_BODY()
public:

	bool ValidateLocation(FVector& LocationToValidate) const;

protected:
	UPROPERTY(EditAnywhere, Category = CombatArea)
	ASecCombatArea* CombatArea = nullptr;
};
