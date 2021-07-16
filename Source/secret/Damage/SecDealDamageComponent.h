#pragma once

#include "Components/ActorComponent.h"
#include "SecDamageStatics.h"
#include "SecDealDamageComponent.generated.h"

class USecTakeDamageComponent;

UCLASS()
class SECRET_API USecDealDamageComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	USecDealDamageComponent();

	void DealPointDamageToActor(AActor* TargetActor, const FSecPointDamage& Damage, FSecDamageOutput& DamageOutput);

private:
	USecTakeDamageComponent* GetTakeDamageComponentFromActor(AActor* TargetActor) const;

	UFUNCTION(Server, Reliable)
	void Server_DealDamageToActor(AActor* TargetActor, const FSecPointDamage& Damage);
};
