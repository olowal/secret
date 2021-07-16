#include "SecTakeDamageComponent.h"
#include "GameFramework/Pawn.h"

void USecTakeDamageComponent::ProcessDamage(const FSecDamageInput& InInput, FSecDamageOutput& Output) const
{
	TArray<const USecDamageModifier*> TempModifierList;
	ModifierList.GetSortedList(TempModifierList);

	FSecDamageInput Input;
	Input.DamageDealer = InInput.DamageDealer;
	Input.DamageOrigin = InInput.DamageOrigin;
	Output.DamageDealt = InInput.Damage;
	Output.DamageType = InInput.DamageType;

	for (const USecDamageModifier* DamageModifier : TempModifierList)
	{
		if (!ensure(DamageModifier != nullptr))
			continue;

		DamageModifier->ProcessDamageOutput(FSecDamageInput(Output, Input), Output);
	}
}

void USecTakeDamageComponent::ProcessDamagePrediction(const FSecDamageInput& Input, FSecDamageOutput& Output)
{
	ProcessDamage(Input, Output);
}

void USecTakeDamageComponent::AddDamageModifier(TSubclassOf<USecDamageModifier> DamageModifierClass, UObject* Instigator, ESecScriptPriority Priority)
{
	if (!ensureMsgf(DamageModifierClass.GetDefaultObject() != nullptr, TEXT("Invalid DamageModifierClass")))
		return;

	const FName DefaultObjectName = DamageModifierClass->GetDefaultObjectName();
	USecDamageModifier** DamageModifierPtr = CachedInstances.Find(DefaultObjectName);

	if (DamageModifierPtr != nullptr)
	{
		USecDamageModifier* DamageModifier = (*DamageModifierPtr);

		if (!ensureMsgf(!DamageModifier->IsApplied(), TEXT("Attempting to apply %s when it is already applied."), *DefaultObjectName.ToString()))
			return;

		DamageModifier->OnApply();
		ModifierList.Add(DamageModifier, Instigator, Priority);
	}
	else
	{
		USecDamageModifier* DamageModifier = NewObject<USecDamageModifier>(GetOwner(), DamageModifierClass);
		check(DamageModifier != nullptr);
		CachedInstances.Add(DefaultObjectName, DamageModifier);
		DamageModifier->Owner = GetOwner();
		DamageModifier->Setup();
		DamageModifier->OnApply();
		ModifierList.Add(DamageModifier, Instigator, Priority);
	}
}

void USecTakeDamageComponent::RemoveDamageModifierByClass(TSubclassOf<USecDamageModifier> DamageModifierClass)
{
	if (!ensureMsgf(DamageModifierClass.GetDefaultObject() != nullptr, TEXT("Invalid DamageModifierClass")))
		return;

	const FName DefaultObjectName = DamageModifierClass->GetDefaultObjectName();
	USecDamageModifier** DamageModifierPtr = CachedInstances.Find(DefaultObjectName);

	if (!ensureMsgf(DamageModifierPtr != nullptr, TEXT("Attempting to remove %s, but it is already removed or does not exist."), *DefaultObjectName.ToString()))
		return;

	USecDamageModifier* DamageModifier = (*DamageModifierPtr);
	DamageModifier->OnRemove();
	ModifierList.RemoveByObject(DamageModifier);
}

void USecTakeDamageComponent::RemoveDamageModifierByInstigator(UObject* Instigator)
{
	TArray<USecDamageModifier*> InDamageModifiers;
	ModifierList.GetObjectsByInstigator(InDamageModifiers, Instigator);

	if (!ensureMsgf(InDamageModifiers.Num() != 0, TEXT("Unable to locate DamageModifier to remove.")))
		return;

	USecDamageModifier* DamageModifier = InDamageModifiers[0];
	check(DamageModifier != nullptr);

	ensureMsgf(InDamageModifiers.Num() == 1, TEXT("More than one instance of %s was found when attempting to remove this Damage Modifier."), *DamageModifier->GetName());

	DamageModifier->OnRemove();
	ModifierList.RemoveByInstigator(Instigator);
}

bool USecTakeDamageComponent::Prediction() const
{
	if (const AActor* OwnerActor = GetOwner())
	{
		OwnerActor->GetLocalRole() == ROLE_AutonomousProxy && !OwnerActor->HasAuthority();
	}

	return false;
}

bool USecTakeDamageComponent::IsLocallyControlled() const
{
	if (const APawn* OwnerPawn = Cast<APawn>(GetOwner()))
	{
		return OwnerPawn->IsLocallyControlled();
	}

	return HasAuthority();
}

bool USecTakeDamageComponent::HasAuthority() const
{
	return GetOwner()->HasAuthority();
}
