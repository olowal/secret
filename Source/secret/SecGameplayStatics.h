#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Capability/SecScriptPriority.h"
#include "SecGameplayStatics.generated.h"

class USecCapabilitySheet;

UCLASS()
class SECRET_API USecGameplayStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Utilities", meta = (WorldContext = "WorldContextObject"))
	static class ASecThirdPersonCameraBase* GetThirdPersonCamera(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "Utilities")
	static void DealDamageToActor(class AActor* DamagedActor, float BaseDamage, class AController* EventInstigator, class AActor* DamageCauser, TSubclassOf<class UDamageType> DamageTypeClass);

	UFUNCTION(BlueprintCallable, Category = "Utilities", meta = (WorldContext = "WorldContextObject"))
	static void AddCapabilitySheetToActor(const UObject* WorldContextObject, AActor* TargetActor, USecCapabilitySheet* Sheet, ESecScriptPriority Priority);

private:
	static bool HasAuthority(const UObject* WorldContextObject);
};
