#pragma once

#include "Engine/DataAsset.h"
#include "SecAttributeStatics.h"
#include "SecAttributeSheet.generated.h"

class USecAttribute;

UCLASS()
class SECRET_API USecAttributeSheet : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly)
	TMap<TSubclassOf<USecAttribute>, FSecAttributeOverrideData> Attributes;
};
