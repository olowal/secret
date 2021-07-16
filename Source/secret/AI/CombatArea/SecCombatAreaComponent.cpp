#include "SecCombatAreaComponent.h"
#include "AI/CombatArea/SecCombatArea.h"

bool USecCombatAreaComponent::ValidateLocation(FVector& LocationToValidate) const
{
	if (CombatArea != nullptr)
	{
		return CombatArea->ValidateLocation(LocationToValidate);
	}

	return true;
}
