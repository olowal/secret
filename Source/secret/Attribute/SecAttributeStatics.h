#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "SecAttributeStatics.generated.h"

class USecAttribute;

UENUM(BlueprintType)
enum class ESecApplyAttributeModifierType : uint8
{
	AllPriorities,	// Will apply in the order low -> highest
	OnlyHighestPriority // Only apply all attribute modifiers of highest priority
};

UENUM(BlueprintType)
enum class ESecAttributeClampType : uint8
{
	None,
	Value,
	Attribute
};

USTRUCT(BlueprintType)
struct FSecAttributeClampData
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attribute)
	ESecApplyAttributeModifierType ApplyAttributeModifierType = ESecApplyAttributeModifierType::AllPriorities;
	UPROPERTY(EditDefaultsOnly)
	ESecAttributeClampType ClampMin = ESecAttributeClampType::None;
	UPROPERTY(EditDefaultsOnly)
	ESecAttributeClampType ClampMax = ESecAttributeClampType::None;

	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "ClampMin == ESecAttributeClampType::Attribute", EditConditionHides))
	TSubclassOf<USecAttribute> ClampMinClass;
	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "ClampMax == ESecAttributeClampType::Attribute", EditConditionHides))
	TSubclassOf<USecAttribute> ClampMaxClass;
	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "ClampMin == ESecAttributeClampType::Value", EditConditionHides))
	float ClampMinValue = 0.0f;
	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "ClampMax == ESecAttributeClampType::Value", EditConditionHides))
	float ClampMaxValue = 0.0f;
};

USTRUCT(BlueprintType)
struct FSecAttributeOverrideData
{
	GENERATED_BODY()
public:
	FSecAttributeOverrideData(){}

	UPROPERTY(EditAnywhere, meta = (EditCondition = "bOverrideClampData", EditConditionHides, DisplayAfter = "bOverrideClampData"))
	FSecAttributeClampData ClampData;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bOverrideDefaultValue", EditConditionHides, DisplayAfter = "bOverrideDefaultValue"))
	float DefaultValue = 1.0f;
	UPROPERTY(EditAnywhere)
	bool bOverrideDefaultValue = false;
	UPROPERTY(EditAnywhere)
	bool bOverrideClampData = false;
};


UCLASS()
class SECRET_API USecAttributeStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	//UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Objective, meta = (WorldContext = "WorldContextObject"))
	//static FAttributeModifierInfo CreateModifierInfo(const UObject* WorldContextObject, FName AttributeName, EAttributeModifierType ModifierType, float Value);
};
