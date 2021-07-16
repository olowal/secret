#include "SecDealDamageComponent.h"
#include "Network/SecNetworkStatics.h"
#include "SecTakeDamageComponent.h"

USecDealDamageComponent::USecDealDamageComponent()
{
	
}

void USecDealDamageComponent::DealPointDamageToActor(AActor* TargetActor, const FSecPointDamage& Damage, FSecDamageOutput& DamageOutput)
{
	if (TargetActor == nullptr)
		return;

	const bool bUsePrediction = USecNetworkStatics::UsePredictionForActor(TargetActor);

	if (bUsePrediction)
	{
		if (USecTakeDamageComponent* TakeDamageComponent = GetTakeDamageComponentFromActor(TargetActor))
		{
			TakeDamageComponent->ProcessDamagePrediction(Damage.DamageInput, DamageOutput);
		}
	}

	Server_DealDamageToActor(TargetActor, Damage);
}

USecTakeDamageComponent* USecDealDamageComponent::GetTakeDamageComponentFromActor(AActor* TargetActor) const
{
	return TargetActor->FindComponentByClass<USecTakeDamageComponent>();
}

void USecDealDamageComponent::Server_DealDamageToActor_Implementation(AActor* TargetActor, const FSecPointDamage& Damage)
{
	if (TargetActor == nullptr)
		return;

	USecTakeDamageComponent* TakeDamageComponent = GetTakeDamageComponentFromActor(TargetActor);

	
}
