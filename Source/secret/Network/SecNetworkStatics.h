#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "SecNetworkStatics.generated.h"

UCLASS()
class SECRET_API USecNetworkStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure, Category = Network)
	static bool IsLocallyControlled(AActor* TargetActor);

	UFUNCTION(BlueprintPure, Category = Network)
	static bool UsePredictionForActor(AActor* TargetActor);
};
