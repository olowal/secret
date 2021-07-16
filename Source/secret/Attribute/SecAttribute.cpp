#include "SecAttribute.h"
#include "SecAttributeModifierSheet.h"
#include "SecAttributeModifierInterface.h"
#include "SecAttributeComponent.h"

void USecAttribute::PreSetup(const FSecAttributeOverrideData& AttributeOverrideData)
{
	if (AttributeOverrideData.bOverrideClampData)
	{
		ClampData = AttributeOverrideData.ClampData;
	}

	if (AttributeOverrideData.bOverrideDefaultValue)
	{
		AttributeValue = AttributeOverrideData.DefaultValue;
	}
}

void USecAttribute::Setup()
{
	ClampMinInstance = ClampMinInstance != nullptr ? ClampMinInstance : AttributeComponent->GetAttributeByClass(ClampData.ClampMinClass);
	ClampMaxInstance = ClampMaxInstance != nullptr ? ClampMaxInstance : AttributeComponent->GetAttributeByClass(ClampData.ClampMaxClass);
	AttributeValue = ClampValue(AttributeValue);
	FinalAttributeValue = AttributeValue;
}

float USecAttribute::GetAttributeValue() const
{
#if !UE_BUILD_SHIPPING
	if (bAlwaysCalculateAttributeValue)
	{
		return CalculateModifier();
	}
	else
	{
		return FinalAttributeValue;
	}
#else
	return FinalAttributeValue;
#endif // !UE_BUILD_SHIPPING
}

float USecAttribute::Internal_GetAttributeBaseValue() const
{
	return AttributeValue + BaseModifier;
}

void USecAttribute::ApplyModifier(UObject* ModifierObject, UObject* InInstigator, ESecScriptPriority Priority)
{
	AttributeModifiers.Add(ModifierObject, InInstigator, Priority);
	CalculateFinalAttributeValue();
}

void USecAttribute::CalculateFinalAttributeValue()
{
	const float OldValue = FinalAttributeValue;
	float NewValue = CalculateModifier();
	BP_PreModifyValue(NewValue, OldValue);

	NewValue = ClampValue(NewValue);

	FinalAttributeValue = NewValue;
	OnModified.Broadcast(NewValue, OldValue);
}

float USecAttribute::ClampValue(float InValue) const
{
	if (ClampData.ClampMin == ESecAttributeClampType::Attribute
		&& ensure(ClampMinInstance != nullptr))
	{
		const float MinValue = ClampMinInstance->GetAttributeValue();
		InValue = FMath::Max(InValue, MinValue);
	}
	else if (ClampData.ClampMin == ESecAttributeClampType::Value)
	{
		InValue = FMath::Max(InValue, ClampData.ClampMinValue);
	}

	if (ClampData.ClampMax == ESecAttributeClampType::Attribute
		&& ensure(ClampMaxInstance != nullptr))
	{
		const float MaxValue = ClampMaxInstance->GetAttributeValue();
		InValue = FMath::Min(InValue, MaxValue);
	}
	else if (ClampData.ClampMax == ESecAttributeClampType::Value)
	{
		InValue = FMath::Min(InValue, ClampData.ClampMaxValue);
	}

	return InValue;
}

void USecAttribute::ModifyBaseValue(float NewBaseValue)
{
	BaseModifier += NewBaseValue;
	CalculateFinalAttributeValue();
}

void USecAttribute::RemoveModifierByInstigator(UObject* Instigator)
{
	AttributeModifiers.RemoveByInstigator(Instigator);
	CalculateFinalAttributeValue();
}

void USecAttribute::RemoveModifierByAsset(USecAttributeModifierSheet* ModifierSheet)
{
	AttributeModifiers.RemoveByObject(ModifierSheet);
	CalculateFinalAttributeValue();
}

float USecAttribute::CalculateModifier() const
{
	if (AttributeModifiers.IsEmpty())
		return Internal_GetAttributeBaseValue();

	TArray<const UObject*> ActiveModifiers;
	
	if (ClampData.ApplyAttributeModifierType == ESecApplyAttributeModifierType::AllPriorities)
	{
		AttributeModifiers.GetSortedList<UObject>(ActiveModifiers);
	}
	else if (ClampData.ApplyAttributeModifierType == ESecApplyAttributeModifierType::OnlyHighestPriority)
	{
		AttributeModifiers.GetHighestPriorityList<UObject>(ActiveModifiers);
	}
#if !UE_BUILD_SHIPPING
	else
	{
		// what?? How did we get here...
		ensureMsgf(false, TEXT("Non-specified ApplyAttributeModifierType has been detected... wtf? Current: %i."), static_cast<int32>(ClampData.ApplyAttributeModifierType));
		return 1.0f;
	}
#endif // !UE_BUILD_SHIPPING
	
	float Value = Internal_GetAttributeBaseValue();
	TArray<FSecAttributeModifierContainer> ModifierList;
	TArray<FSecAttributeModifierContainer> OutModifierList;
	
	for (const UObject* InterfaceObject : ActiveModifiers)
	{
		OutModifierList.Reset();
		ISecAttributeModifierInterface::Execute_GetAttributeModifiers(InterfaceObject, OutModifierList);
		ModifierList.Append(OutModifierList);
	}

	for (const FSecAttributeModifierContainer& Modifier : ModifierList)
	{
		if (IsA(Modifier.Attribute))
			ApplyModifier(Value, Modifier);
	}

	return Value;
}

void USecAttribute::ApplyModifier(float& Value, const FSecAttributeModifierContainer& Modifier) const
{
	const ESecAttributeModifierType Type = Modifier.ModifierType;

	if (Type == ESecAttributeModifierType::Add)
	{
		Value = Value + Modifier.Modifier;
	}
	else if (Type == ESecAttributeModifierType::Sub)
	{
		Value = Value - Modifier.Modifier;
	}
	else if (Type == ESecAttributeModifierType::Mul)
	{
		Value = Value * Modifier.Modifier;
	}
	else if (Type == ESecAttributeModifierType::Div)
	{
		Value = Value / Modifier.Modifier;
	}
	else if (Type == ESecAttributeModifierType::Custom)
	{
		// TODO: Whenever
	}
}

