#pragma once

#include "Components/ActorComponent.h"
#include "SecDamageStatics.h"
#include "Utils/SecPriorityList.h"
#include "SecTakeDamageComponent.generated.h"

UCLASS()
class SECRET_API USecTakeDamageComponent : public UActorComponent
{
	GENERATED_BODY()
public:

	void ProcessDamage(const FSecDamageInput& Input, FSecDamageOutput& Output) const;
	void ProcessDamagePrediction(const FSecDamageInput& Input, FSecDamageOutput& Output);

	UFUNCTION(BlueprintCallable, Category = Damage)
	void AddDamageModifier(TSubclassOf<USecDamageModifier> DamageModifierClass, UObject* Instigator, ESecScriptPriority Priority);
	UFUNCTION(BlueprintCallable, Category = Damage)
	void RemoveDamageModifierByClass(TSubclassOf<USecDamageModifier> DamageModifierClass);
	UFUNCTION(BlueprintCallable, Category = Damage)
	void RemoveDamageModifierByInstigator(UObject* Instigator);

	bool Prediction() const;
	bool IsLocallyControlled() const;
	bool HasAuthority() const;

private:
	UPROPERTY(Transient)
	FSecPriorityList ModifierList;

	UPROPERTY(Transient)
	TMap<FName, USecDamageModifier*> CachedInstances;

	// Only accumulate only locally controlled autonomous proxy
	UPROPERTY(Transient)
	TArray<FSecDamageOutput> DamagePrediction;
};
