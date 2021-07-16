#include "SecTriggerComponent.h"
#include "GameFramework/Volume.h"
#include "Components/BrushComponent.h"
#include "SecTriggerUserComponent.h"

void USecTriggerComponent::AddActionVolume(AVolume* Volume)
{
	if (!ensure(Volume != nullptr))
		return;

	BindToActionPrimitive(Volume->GetBrushComponent());
}

static const FName NAME_ActionShapeCollisionProfile("TriggerOnlyPlayer");
void USecTriggerComponent::AddActionShape(FSecShapeSettings ShapeSettings, const FTransform& ShapeTransform)
{
	auto* CreatedComponent = USecShapeStatics::CreateTriggerShape(GetOwner(), this, ShapeSettings, ShapeTransform, NAME_ActionShapeCollisionProfile);
	BindToActionPrimitive(CreatedComponent);
}

void USecTriggerComponent::AddActionPrimitive(UPrimitiveComponent* Primitive)
{
	BindToActionPrimitive(Primitive);
}

void USecTriggerComponent::AddFocusVolume(AVolume* Volume)
{
	if (!ensure(Volume != nullptr))
		return;

	BindToFocusPrimitive(Volume->GetBrushComponent());
}

void USecTriggerComponent::AddFocusShape(FSecShapeSettings ShapeSettings, const FTransform& ShapeTransform)
{
	auto* CreatedComponent = USecShapeStatics::CreateTriggerShape(GetOwner(), this, ShapeSettings, ShapeTransform, NAME_ActionShapeCollisionProfile);
	BindToFocusPrimitive(CreatedComponent);
}

void USecTriggerComponent::AddFocusPrimitive(UPrimitiveComponent* Primitive)
{
	BindToFocusPrimitive(Primitive);
}

void USecTriggerComponent::Disable(FName Reason)
{
	if (!DisableReasons.Contains(Reason))
	{
		DisableReasons.Add(Reason);
	}
}

void USecTriggerComponent::Enable(FName Reason)
{
	DisableReasons.Remove(Reason);
}

void USecTriggerComponent::DisableForCharacter(ASecCharacter* Character, FName Reason)
{
	FSecTriggerDisableReasonContainer DisableReasonCharacter = FSecTriggerDisableReasonContainer(Character, Reason);
	if (!CharacterDisableReasons.Contains(DisableReasonCharacter))
	{
		CharacterDisableReasons.Add(DisableReasonCharacter);
	}
}

void USecTriggerComponent::EnableForCharacter(ASecCharacter* Character, FName Reason)
{
	FSecTriggerDisableReasonContainer DisableReasonCharacter = FSecTriggerDisableReasonContainer(Character, Reason);
	CharacterDisableReasons.Remove(DisableReasonCharacter);
}

bool USecTriggerComponent::IsDisabledForCharacter(FName Reason, ASecCharacter* ForCharacter)
{
	FSecTriggerDisableReasonContainer DisableReasonCharacter = FSecTriggerDisableReasonContainer(ForCharacter, Reason);
	return CharacterDisableReasons.Contains(DisableReasonCharacter);
}

bool USecTriggerComponent::IsDisabled(FName Reason)
{
	return DisableReasons.Contains(Reason);
}

bool USecTriggerComponent::IsDisabledForAnyReason()
{
	return DisableReasons.Num() > 0;
}

bool USecTriggerComponent::IsDisabledForCharacterForAnyReason(ASecCharacter* ForCharacter)
{
	return false;
}

FSecTriggerConditionHandle USecTriggerComponent::AddTriggerCondition(FName Reason, FSecTriggerCondition Condition)
{
	return FSecTriggerConditionHandle{ -1 };
}

FSecTriggerConditionHandle USecTriggerComponent::AddCharacterCondition(ASecCharacter* ForCharacter, FName Reason, FSecTriggerCondition Condition)
{
	if (ForCharacter == nullptr)
		return FSecTriggerConditionHandle{ -1 };
	return FSecTriggerConditionHandle{ -1 };
}

void USecTriggerComponent::UpdateConditions()
{
	for (FSecTriggerConditionContainer& It : Conditions)
	{
		//UpdateConditionsForCharacter(It->Character);
	}
}

void USecTriggerComponent::UpdateConditionsForCharacter(FName Reason, ASecCharacter* Character)
{

}

void USecTriggerComponent::AddActivationDelegate(FSecTriggerActivationDelegate Delegate)
{
	ActivationDelegates.Add(Delegate);
}

void USecTriggerComponent::AddActivationDelegate(FSecTriggerActivationCodeDelegate Delegate)
{
	ActivationCodeDelegates.Add(Delegate);
}

void USecTriggerComponent::StartActivating(ASecCharacter* Character)
{
	// Implement network activation
}

void USecTriggerComponent::BindToActionPrimitive(UPrimitiveComponent* Component)
{
	if (!ensure(Component != nullptr))
		return;

	// Bind overlap events
	Component->OnComponentBeginOverlap.AddUniqueDynamic(this, &USecTriggerComponent::OnActionPrimitiveBeginOverlap);
	Component->OnComponentEndOverlap.AddUniqueDynamic(this, &USecTriggerComponent::OnActionPrimitiveEndOverlap);

	// Trigger existing overlaps
	TArray<AActor*> OverlappingActors;
	Component->GetOverlappingActors(OverlappingActors);

	for (AActor* OtherActor : OverlappingActors)
		EnterActionArea(Component, OtherActor);
}

void USecTriggerComponent::OnActionPrimitiveBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	EnterActionArea(OverlappedComponent, OtherActor);
}

void USecTriggerComponent::OnActionPrimitiveEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	LeaveActionArea(OverlappedComponent, OtherActor);
}

void USecTriggerComponent::EnterActionArea(UPrimitiveComponent* ActionArea, AActor* EnteredActor)
{
	if (auto* TriggerUser = EnteredActor->FindComponentByClass<USecTriggerUserComponent>())
	{
		auto& Overlapping = ActionAreaOverlaps.FindOrAdd(EnteredActor);

		if (Overlapping.Num() == 0)
		{
			// First action area overlap, add to trigger user
			Overlapping.Add(ActionArea);
			TriggerUser->AddAvailableTriggerComponent(this);
		}
		else
		{
			Overlapping.AddUnique(ActionArea);
		}
	}
}

void USecTriggerComponent::LeaveActionArea(UPrimitiveComponent* ActionArea, AActor* LeftActor)
{
	auto* Overlapping = ActionAreaOverlaps.Find(LeftActor);
	if (Overlapping != nullptr)
	{
		Overlapping->Remove(ActionArea);

		if (Overlapping->Num() == 0)
		{
			// No longer overlapping action area, remove from trigger user
			if (auto* TriggerUser = LeftActor->FindComponentByClass<USecTriggerUserComponent>())
			{
				TriggerUser->RemoveAvailableTriggerComponent(this);
			}

			ActionAreaOverlaps.Remove(LeftActor);
		}
	}
}

void USecTriggerComponent::BindToFocusPrimitive(UPrimitiveComponent* Component)
{
	if (!ensure(Component != nullptr))
		return;

	// Bind overlap events
	Component->OnComponentBeginOverlap.AddUniqueDynamic(this, &USecTriggerComponent::OnFocusPrimitiveBeginOverlap);
	Component->OnComponentEndOverlap.AddUniqueDynamic(this, &USecTriggerComponent::OnFocusPrimitiveEndOverlap);

	// Trigger existing overlaps
	TArray<AActor*> OverlappingActors;
	Component->GetOverlappingActors(OverlappingActors);

	for (AActor* OtherActor : OverlappingActors)
		EnterFocusArea(Component, OtherActor);
}

void USecTriggerComponent::OnFocusPrimitiveBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	EnterFocusArea(OverlappedComponent, OtherActor);
}

void USecTriggerComponent::OnFocusPrimitiveEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	LeaveFocusArea(OverlappedComponent, OtherActor);
}

void USecTriggerComponent::EnterFocusArea(UPrimitiveComponent* FocusArea, AActor* EnteredActor)
{
	if (auto* TriggerUser = EnteredActor->FindComponentByClass<USecTriggerUserComponent>())
	{
		auto& Overlapping = FocusAreaOverlaps.FindOrAdd(EnteredActor);

		if (Overlapping.Num() == 0)
		{
			// First action area overlap, add to trigger user
			Overlapping.Add(FocusArea);
			TriggerUser->AddVisibleTriggerComponent(this);
		}
		else
		{
			Overlapping.AddUnique(FocusArea);
		}
	}
}

void USecTriggerComponent::LeaveFocusArea(UPrimitiveComponent* FocusArea, AActor* LeftActor)
{
	auto* Overlapping = FocusAreaOverlaps.Find(LeftActor);
	if (Overlapping != nullptr)
	{
		Overlapping->Remove(FocusArea);

		if (Overlapping->Num() == 0)
		{
			// No longer overlapping action area, remove from trigger user
			if (auto* TriggerUser = LeftActor->FindComponentByClass<USecTriggerUserComponent>())
			{
				TriggerUser->RemoveVisibleTriggerComponent(this);
			}

			FocusAreaOverlaps.Remove(LeftActor);
		}
	}
}
