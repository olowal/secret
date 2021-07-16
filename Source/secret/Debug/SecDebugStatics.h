#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "SecDebugStatics.generated.h"

class FPrimitiveDrawInterface;

UCLASS()
class SECRET_API USecDebugStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	//static void DrawDebugCone()

private:
	static void DrawLine(UWorld* World, const FVector& Start, const FVector& End, const FLinearColor& Color, float Thickness, bool bPersistentLines, float Lifetime, FPrimitiveDrawInterface* PDI = nullptr);
};
