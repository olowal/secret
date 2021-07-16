#pragma once

#include "Engine/DataAsset.h"
#include "SecShieldSchematic.generated.h"

UCLASS(Blueprintable)
class SECRET_API USecShieldSchematic : public UDataAsset
{
	GENERATED_BODY()
public:

	UPROPERTY(EditDefaultsOnly, Category = "Shield")
	TSubclassOf<class ASecShieldBase> ShieldActor;
};
