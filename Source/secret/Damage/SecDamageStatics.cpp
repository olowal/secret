#include "SecDamageStatics.h"
#include "SecTakeDamageComponent.h"
#include "SecDealDamageComponent.h"

void USecDamageModifier::Setup()
{
	BP_Setup();
}

void USecDamageModifier::OnApply()
{
	bIsApplied = true;
	BP_OnApply();
}

void USecDamageModifier::OnRemove()
{
	bIsApplied = false;
	BP_OnRemove();
}

void USecDamageModifier::ProcessDamageOutput(const FSecDamageInput& Input, FSecDamageOutput& Output) const
{
	BP_ProcessDamageOutput(Input, Output);
}

bool USecDamageModifier::IsApplied() const
{
	return bIsApplied;
}

void USecLineDamageStatics::DealDamageTo(const FSecPointDamage& DamageType_Line, AActor* TargetActor)
{
	if (!ensure(TargetActor != nullptr))
		return;

	USecTakeDamageComponent* TakeDamageComponent = USecDamageStatics::GetOrCreateTakeDamageComponent(TargetActor);
	check(TakeDamageComponent != nullptr);



}

USecTakeDamageComponent* USecDamageStatics::GetOrCreateTakeDamageComponent(AActor* TargetActor)
{
	check(TargetActor != nullptr);

	USecTakeDamageComponent* TakeDamageComponent = TargetActor->FindComponentByClass<USecTakeDamageComponent>();

	if (TakeDamageComponent == nullptr)
	{
		TakeDamageComponent = NewObject<USecTakeDamageComponent>(TargetActor);
		TakeDamageComponent->RegisterComponent();
	}

	return TakeDamageComponent;
}

USecDealDamageComponent* USecDamageStatics::GetOrCreateDealDamageComponent(AActor* DamageDealer)
{
	check(DamageDealer != nullptr);

	USecDealDamageComponent* DealDamageComponent = DamageDealer->FindComponentByClass<USecDealDamageComponent>();

	if (DealDamageComponent == nullptr)
	{
		DealDamageComponent = NewObject<USecDealDamageComponent>(DamageDealer);
		DealDamageComponent->RegisterComponent();
	}

	return DealDamageComponent;
}
