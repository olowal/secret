#pragma once

#include "Engine/DataAsset.h"
#include "SecAttributeModifier.h"
#include "SecAttributeModifierInterface.h"
#include "SecAttributeModifierSheet.generated.h"

UCLASS()
class SECRET_API USecAttributeModifierSheet : public UDataAsset, public ISecAttributeModifierInterface
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, Category = Modifier)
	TArray<FSecAttributeModifierContainer> ModifierList;

	UFUNCTION(BlueprintNativeEvent, Category = Attribute)
	bool GetAttributeModifiers(TArray<FSecAttributeModifierContainer>& OutAttributeModifiers) const;
	virtual bool GetAttributeModifiers_Implementation(TArray<FSecAttributeModifierContainer>& OutAttributeModifiers) const override;

};

UCLASS()
class SECRET_API USecAttributeModifierDataHolder : public UObject, public ISecAttributeModifierInterface
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent, Category = Attribute)
	bool GetAttributeModifiers(TArray<FSecAttributeModifierContainer>& OutAttributeModifiers) const;
	virtual bool GetAttributeModifiers_Implementation(TArray<FSecAttributeModifierContainer>& OutAttributeModifiers) const override;

	FSecAttributeModifierContainer ModifierContainer;
};
