#include "SecAttributeModifierSheet.h"

bool USecAttributeModifierSheet::GetAttributeModifiers_Implementation(TArray<FSecAttributeModifierContainer>& OutAttributeModifiers) const
{
	for (const FSecAttributeModifierContainer& AMC : ModifierList)
	{
		OutAttributeModifiers.Add(AMC);
	}

	return true;
}

bool USecAttributeModifierDataHolder::GetAttributeModifiers_Implementation(TArray<FSecAttributeModifierContainer>& OutAttributeModifiers) const
{
	OutAttributeModifiers.Add(ModifierContainer);
	return true;
}
