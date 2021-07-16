#include "SecAttributeModifier.h"
/*
void UAttributeModifier::ApplyModifiers(FName AttributeName, float& Attribute)
{
	for (const FAttributeModifierInfo& Modifier : Modifiers)
	{
		if (Modifier.AttributeName != AttributeName)
		{
			continue;
		}

		if (Modifier.ModifierType == EAttributeModifierType::Add)
		{
			Attribute = Attribute + Modifier.Value;
		}
		else if (Modifier.ModifierType == EAttributeModifierType::Sub)
		{
			Attribute = Attribute - Modifier.Value;
		}
		else if (Modifier.ModifierType == EAttributeModifierType::Mul)
		{
			Attribute = Attribute * Modifier.Value;
		}
		else if (Modifier.ModifierType == EAttributeModifierType::Div)
		{
			Attribute = Attribute / Modifier.Value;
		}
		else if (Modifier.ModifierType == EAttributeModifierType::Override)
		{
			Attribute = Modifier.Value;
		}
	}
}

void UAttributeModifier::AddModifier(FName InAttributeName, EAttributeModifierType InModifierType, float InValue)
{
	Modifiers.Add(FAttributeModifierInfo(InAttributeName, InModifierType, InValue));
}

void USecAttribute::RecalculateModifier()
{

}

float USecAttribute::CalculateModifier() const
{
	float Modifier = 1.0f;
}

float USecAttribute::GetAttribute() const
{
	return Value * CalculateModifier();
}

void USecAttribute::ApplyAttributeSheet(USecAttributeSheet* AttributeSheet, int32 Priority)
{

}

void USecAttribute::RemoveAttributeSheet(USecAttributeSheet* AttributeSheet)
{
	SheetAttributeList.Remove(AttributeSheet);
}
*/

struct FSecAttributeModifierContainer USecAttributeModifierContainerStatics::Normal(float InModifier)
{
	FSecAttributeModifierContainer AttributeModifier;
	AttributeModifier.Modifier = InModifier;
	return AttributeModifier;
}
