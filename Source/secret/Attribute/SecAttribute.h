#pragma once

#include "UObject/Object.h"
#include "Utils/SecPriorityList.h"
#include "SecAttributeModifierType.h"
#include "SecAttributeStatics.h"
#include "SecAttribute.generated.h"

class USecAttributeComponent;
class USecAttributeModifierSheet;
struct FSecAttributeModifierContainer;



DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSecAttribute_OnModified, float, NewValue, float, OldValue);

UCLASS(BlueprintType, Blueprintable)
class SECRET_API USecAttribute : public UObject
{
	GENERATED_BODY()
public:
	// Runs right after attribute is instantiated.
	void PreSetup(const FSecAttributeOverrideData& AttributeOverrideData);

	// Runs when all attributes in the collection has been instantiated.
	void Setup();

	// Returns the base value with applied modifiers
	UFUNCTION(BlueprintPure, Category = Attribute)
	float GetAttributeValue() const;
	UFUNCTION(BlueprintPure, Category = Attribute)
	float GetBaseValue() const { return AttributeValue; }

	void ApplyModifier(UObject* ModifierObject, UObject* InInstigator, ESecScriptPriority Priority);
	// Alters the modifier for the base value. Expects a value positive or negative value.
	UFUNCTION(BlueprintCallable, Category = Attribute)
	void ModifyBaseValue(float InModifier);
	void RemoveModifierByInstigator(UObject* Instigator);
	void RemoveModifierByAsset(USecAttributeModifierSheet* ModifierSheet);

	float CalculateModifier() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Debug)
	bool bAlwaysCalculateAttributeValue = true;

	UPROPERTY(BlueprintAssignable)
	FSecAttribute_OnModified OnModified;

	// Before clamping the new modified value
	UFUNCTION(BlueprintImplementableEvent, Category = Attribute, meta = (DisplayName = "PreModifyValue"))
	void BP_PreModifyValue(float& NewValue, float OldValue);

	UPROPERTY(Transient)
	USecAttributeComponent* AttributeComponent = nullptr;

	void CalculateFinalAttributeValue();

	float ClampValue(float InValue) const;

protected:
	UPROPERTY(EditDefaultsOnly)
	FSecAttributeClampData ClampData;

	// This returns the base attribute value subtracted by the base modifier. Used internally for final value calculations.
	float Internal_GetAttributeBaseValue() const;

	void ApplyModifier(float& Value, const FSecAttributeModifierContainer& Modifier) const;
	
	FSecPriorityList AttributeModifiers;

	// Base value for this attribute
	UPROPERTY(EditDefaultsOnly)
	float AttributeValue = 0.0f;

	// Store the already calculated value here
	float FinalAttributeValue = 0.0f;

private:
	

	// When we want to change the base value by simple modifiers.
	float BaseModifier = 0.0f;

	UPROPERTY(Transient)
	USecAttribute* ClampMinInstance = nullptr;
	UPROPERTY(Transient)
	USecAttribute* ClampMaxInstance = nullptr;
};
