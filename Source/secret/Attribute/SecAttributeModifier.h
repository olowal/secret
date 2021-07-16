#pragma once

#include "UObject/Object.h"
#include "SecAttributeModifierType.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SecAttributeModifier.generated.h"

class USecAttribute;

UCLASS(BlueprintType)
class SECRET_API UAttributeModifier : public UObject
{
	GENERATED_BODY()
public:
};

USTRUCT(BlueprintType)
struct FSecAttributeModifierContainer
{
	GENERATED_BODY()
public:
	FSecAttributeModifierContainer(){}
	FSecAttributeModifierContainer(
		float InModifier, 
		TSubclassOf<USecAttribute> InAttribute, 
		ESecAttributeModifierType InModifierType,
		TSubclassOf<UAttributeModifier> InCustomModifierClass = nullptr
	) : 
		Modifier(InModifier), 
		Attribute(InAttribute),
		ModifierType(InModifierType),
		CustomModifierClass(InCustomModifierClass)
	{}

	~FSecAttributeModifierContainer() {}

	UPROPERTY(EditDefaultsOnly, Category = AttributeModifier)
	float Modifier = 0.0f;
	UPROPERTY(EditDefaultsOnly, Category = AttributeModifier)
	TSubclassOf<USecAttribute> Attribute;
	UPROPERTY(EditDefaultsOnly, Category = AttributeModifier)
	ESecAttributeModifierType ModifierType = ESecAttributeModifierType::Mul;
	UPROPERTY(EditDefaultsOnly, Category = AttributeModifier, meta = (EditCondition = "ModifierType == ESecAttributeModifierType::Custom", EditConditionHides))
	TSubclassOf<UAttributeModifier> CustomModifierClass = nullptr;
};

UCLASS(meta = (ScriptMixin = "FSecAttributeModifierContainer"))
class SECRET_API USecAttributeModifierContainerStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = Attribute)
	struct FSecAttributeModifierContainer Normal(float InModifier);
};
