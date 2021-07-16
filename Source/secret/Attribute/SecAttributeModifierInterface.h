#pragma once

#include "UObject/Interface.h"
#include "SecAttributeModifier.h"
#include "CoreMinimal.h"
#include "SecAttributeModifierInterface.generated.h"

struct FSecAttributeModifierContainer;

UINTERFACE()
class SECRET_API USecAttributeModifierInterface : public UInterface
{
	GENERATED_BODY()
};

class SECRET_API ISecAttributeModifierInterface
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent, Category = Attribute)
	bool GetAttributeModifiers(TArray<FSecAttributeModifierContainer>& OutAttributeModifiers) const;
};
