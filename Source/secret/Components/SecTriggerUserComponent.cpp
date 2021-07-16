#include "SecTriggerUserComponent.h"
#include "SecTriggerComponent.h"
#include "Character/SecCharacter.h"

USecTriggerUserComponent::USecTriggerUserComponent(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void USecTriggerUserComponent::AddAvailableTriggerComponent(USecTriggerComponent* TriggerComponent)
{
	AvailableTriggers.Add(TriggerComponent);
}

void USecTriggerUserComponent::RemoveAvailableTriggerComponent(USecTriggerComponent* TriggerComponent)
{
	AvailableTriggers.Remove(TriggerComponent);
}

void USecTriggerUserComponent::AddVisibleTriggerComponent(USecTriggerComponent* TriggerComponent)
{
	VisibleTriggers.Add(TriggerComponent);
}

void USecTriggerUserComponent::RemoveVisibleTriggerComponent(USecTriggerComponent* TriggerComponent)
{
	VisibleTriggers.Remove(TriggerComponent);
}

void USecTriggerUserComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	UpdateAvailableTriggers();
	UpdateVisibleTriggers();
}

USecTriggerComponent* USecTriggerUserComponent::ActivateAnyAvailableTrigger()
{
	ASecCharacter* Character = CastChecked<ASecCharacter>(GetOwner());

	TArray<USecTriggerComponent*> Triggers;
	GetAvailableTriggers(Triggers);

	USecTriggerComponent* Focused = GetFocusedTrigger(Triggers);
	if (Focused != nullptr)
		Focused->StartActivating(Character);

	return Focused;
}

bool USecTriggerUserComponent::GetAvailableTriggers(TArray<USecTriggerComponent*>& OutTriggers)
{
	OutTriggers = AvailableTriggers;
	return true;
}

bool USecTriggerUserComponent::GetAllAvailableTriggers(TArray<USecTriggerComponent*>& OutTriggers)
{
	OutTriggers = AvailableTriggers;
	return true;
}

bool USecTriggerUserComponent::GetVisibleTriggers(TArray<USecTriggerComponent*>& OutTriggers)
{
	OutTriggers = VisibleTriggers;
	return true;
}

bool USecTriggerUserComponent::GetAllVisibleTriggers(TArray<USecTriggerComponent*>& OutTriggers)
{
	OutTriggers = VisibleTriggers;
	return true;
}

bool USecTriggerUserComponent::IsTriggerAvailable(USecTriggerComponent* TriggerComponent)
{
	return true;
}

void USecTriggerUserComponent::SetTriggerRequiredTag(FName Tag)
{
	RequiredActivationTag = Tag;
}

USecTriggerComponent* USecTriggerUserComponent::GetFocusedTrigger(const TArray<USecTriggerComponent*>& Triggers)
{
	USecTriggerComponent* Focused = nullptr;

	// Allow the inherited subclass to decide what to focus,
	// otherwise just choose the first trigger in the list, we don't really have
	// enough information.
	Focused = BP_SelectFocusedTrigger(Triggers);

	if (Focused == nullptr && Triggers.Num() > 0)
		return Triggers[0];
	else
		return Focused;
}

void USecTriggerUserComponent::UpdateAvailableTriggers()
{
	AvailableTriggers.Remove(nullptr);
	for (auto* Trigger : AvailableTriggers)
	{
		if (Trigger == nullptr)
			continue;

		Trigger->UpdateConditions();
	}
}

void USecTriggerUserComponent::UpdateVisibleTriggers()
{

}

bool USecTriggerUserComponent::CanActivateTrigger(USecTriggerComponent* Trigger, bool bForVisuals)
{
	return true;
}
