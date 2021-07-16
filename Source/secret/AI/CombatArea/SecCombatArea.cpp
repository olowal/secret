#include "SecCombatArea.h"

ASecCombatArea::ASecCombatArea(const FObjectInitializer& ObjectInitializer)
{
	RootComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("RootComponent"));
}

bool ASecCombatArea::ValidateLocation(FVector& LocationToValidate) const
{
	return BP_ValidateLocation(LocationToValidate);
}

bool ASecCombatArea::BP_ValidateLocation_Implementation(FVector& LocationToValidate) const
{
	return true;
}
