#pragma once

#include "Components/ActorComponent.h"
#include "Attribute/SecActionState.h"
#include "SecInputComponent.generated.h"

UCLASS()
class SECRET_API USecInputComponent : public UActorComponent
{
	GENERATED_BODY()
public:

	UPROPERTY(Transient, BlueprintReadOnly, Category = Input)
	TMap<FName, float> InputAxis;
	UPROPERTY(Transient, BlueprintReadOnly, Category = Input)
	TMap<FName, ESecActionState> InputAction;
	UPROPERTY(BlueprintReadOnly, Category = Input)
	bool bInitialized = false;
};
