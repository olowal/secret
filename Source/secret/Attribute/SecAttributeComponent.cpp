#include "SecAttributeComponent.h"
#include "GameFramework/Actor.h"
#include "SecAttributeSheet.h"
#include "SecAttribute.h"

USecAttributeComponent::USecAttributeComponent(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.TickGroup = ETickingGroup::TG_PostUpdateWork;
}

void USecAttributeComponent::SetVectorAttribute(FName AttributeName, const FVector& Attribute)
{
	VectorAttributes.FindOrAdd(AttributeName).Set(Attribute.X, Attribute.Y, Attribute.Z);
}

void USecAttributeComponent::SetNumberAttribute(FName AttributeName, float Attribute)
{
	float* bCurrentValue = NumberAttributes.Find(AttributeName);
	if (bCurrentValue != nullptr)
	{
		*bCurrentValue = Attribute;
	}
	else
	{
		NumberAttributes.Add(AttributeName, Attribute);
	}
}

void USecAttributeComponent::SetValueAttribute(FName AttributeName, int32 Attribute)
{
	int32* bCurrentValue = ValueAttributes.Find(AttributeName);
	if (bCurrentValue != nullptr)
	{
		*bCurrentValue = Attribute;
	}
	else
	{
		ValueAttributes.Add(AttributeName, Attribute);
	}
}

void USecAttributeComponent::SetActionAttribute(FName AttributeName, ESecActionState ActionState)
{
	ESecActionState* CurrentValue = ActionAttributes.Find(AttributeName);
	if (CurrentValue != nullptr)
	{
		*CurrentValue = ActionState;
	}
	else
	{
		ActionAttributes.Add(AttributeName, ActionState);
	}
}

void USecAttributeComponent::SetObjectAttribute(FName AttributeName, UObject* ObjectAttribute)
{
	ObjectAttributes.FindOrAdd(AttributeName) = ObjectAttribute;
}

void USecAttributeComponent::ApplyAttributeSheet(USecAttributeSheet* AttributeSheet, UObject* Instigator)
{
	TArray<USecAttribute*> ListOfNewAttributes;

	// First add all attributes
	for (const auto& It : AttributeSheet->Attributes)
	{
		TSubclassOf<USecAttribute> AttributeClass = It.Key;
		const auto& AttributeOverrideData = It.Value;
		USecAttribute* NewAttribute = AddAttribute(AttributeClass);
		
		if (NewAttribute == nullptr)
			continue;
		
		NewAttribute->PreSetup(AttributeOverrideData);
		ListOfNewAttributes.Add(NewAttribute);
	}
	
	// Run setup, this will get possible clamps that are based on other attributes that should now exist.
	for (auto Attribute : ListOfNewAttributes)
		Attribute->Setup();

	// When we have all clamp data ready to go, calculate the starting value
	for (auto Attribute : ListOfNewAttributes)
	{
		Attribute->CalculateFinalAttributeValue();
		OnAttributeInstanced.Broadcast(Attribute, this);
	}
}

void USecAttributeComponent::RemoveAttributeSheetByInstigator(UObject* Instigator)
{

}

void USecAttributeComponent::RemoveAttributeSheetByAsset(USecAttributeSheet* AttributeSheet)
{

}

USecAttribute* USecAttributeComponent::AddAttribute(TSubclassOf<USecAttribute> InAttributeClass)
{
	if (!ensureMsgf(InAttributeClass.GetDefaultObject() != nullptr, TEXT("Invalid USecAttribute class.")))
		return nullptr;
	
	if (!ensureMsgf(!DoesAttributeExist(InAttributeClass), TEXT("%s already exists."), *InAttributeClass.GetDefaultObject()->GetName()))
		return nullptr;

	USecAttribute* NewAttribute = NewObject<USecAttribute>(GetOwner(), InAttributeClass);
	NewAttribute->AttributeComponent = this;
	ListOfAttributes.Add(NewAttribute);
	return NewAttribute;
}

USecAttribute* USecAttributeComponent::AddAttributeFromSheet(TSubclassOf<USecAttribute> InAttributeClass)
{
	if (!ensureMsgf(InAttributeClass.GetDefaultObject() != nullptr, TEXT("Invalid USecAttribute class.")))
		return nullptr;

	if (!ensureMsgf(!DoesAttributeExist(InAttributeClass), TEXT("%s already exists."), *InAttributeClass.GetDefaultObject()->GetName()))
		return nullptr;

	USecAttribute* NewAttribute = NewObject<USecAttribute>(GetOwner(), InAttributeClass);
	NewAttribute->AttributeComponent = this;
	ListOfAttributes.Add(NewAttribute);
	OnAttributeInstanced.Broadcast(NewAttribute, this);
	return NewAttribute;
}

USecAttribute* USecAttributeComponent::GetAttributeByClass(TSubclassOf<USecAttribute> InAttributeClass) const
{
	for (USecAttribute* Attr : ListOfAttributes)
	{
		if (Attr->IsA(InAttributeClass))
			return Attr;
	}

	return nullptr;
}

USecAttribute* USecAttributeComponent::GetOrCreate(TSubclassOf<USecAttribute> InAttributeClass)
{
	if (USecAttribute* Attr = GetAttributeByClass(InAttributeClass))
		return Attr;
	
	 return AddAttribute(InAttributeClass);
}

bool USecAttributeComponent::DoesAttributeExist(TSubclassOf<USecAttribute> InAttributeClass) const
{
	for (USecAttribute* Attribute : ListOfAttributes)
	{
		if (Attribute->IsA(InAttributeClass))
			return true;
	}

	return false;
}

void USecAttributeComponent::ApplyModifierSheet(USecAttributeModifierSheet* AttributeModifierSheet, UObject* InInstigator, ESecScriptPriority Priority)
{
	if (!ensureMsgf(InInstigator != nullptr, TEXT("Invalid Instigator passed to owner %s."), *GetOwner()->GetName()))
		return;
	
	if (!ensureMsgf(AttributeModifierSheet != nullptr, TEXT("Invalid AttributeModifierSheet passed from Instigator: %s in Owner: %s."), *InInstigator->GetName(), *GetOwner()->GetName()))
		return;
	
	ListOfModifiers.Add(AttributeModifierSheet, InInstigator, Priority);

	for (USecAttribute* Attribute : ListOfAttributes)
	{
		for (const FSecAttributeModifierContainer& Modifier : AttributeModifierSheet->ModifierList)
		{
			if (Attribute->IsA(Modifier.Attribute))
			{
				Attribute->ApplyModifier(AttributeModifierSheet, InInstigator, Priority);
				break;
			}
		}
	}
}

void USecAttributeComponent::RemoveModifierSheetByInstigator(UObject* Instigator)
{
	ListOfModifiers.RemoveByInstigator(Instigator);

	for (USecAttribute* Attribute : ListOfAttributes)
	{
		Attribute->RemoveModifierByInstigator(Instigator);
	}
}

void USecAttributeComponent::RemoveModifierSheetByAsset(USecAttributeModifierSheet* AttributeSheet)
{
	ListOfModifiers.RemoveByObject(AttributeSheet);

	for (USecAttribute* Attribute : ListOfAttributes)
	{
		Attribute->RemoveModifierByAsset(AttributeSheet);
	}
}

void USecAttributeComponent::ApplyModifier(const FSecAttributeModifierContainer& AttributeModifier, UObject* InInstigator, ESecScriptPriority Priority)
{
	if (!ensureMsgf(InInstigator != nullptr, TEXT("Invalid Instigator passed to owner %s."), *GetOwner()->GetName()))
		return;
	
	USecAttributeModifierDataHolder* ModifierHolder = NewObject<USecAttributeModifierDataHolder>(GetOwner());
	ensure(ModifierHolder != nullptr);
	ModifierHolder->ModifierContainer = AttributeModifier;
	ListOfModifiers.Add(ModifierHolder, InInstigator, Priority);

	for (USecAttribute* Attribute : ListOfAttributes)
	{
		if (Attribute->IsA(AttributeModifier.Attribute))
			Attribute->ApplyModifier(ModifierHolder, InInstigator, Priority);
	}
}

void USecAttributeComponent::ApplyModifierAndForget(const FSecAttributeModifierContainer& AttributeModifier)
{

}

void USecAttributeComponent::RemoveModifierByInstigator(UObject* InInstigator)
{
	RemoveModifierSheetByInstigator(InInstigator);
}

bool USecAttributeComponent::GetActionAttribute(FName AttributeName) const
{
	return Internal_GetActionAttribute(ActionAttributes, AttributeName);
}

FVector USecAttributeComponent::GetVectorAttribute(FName AttributeName) const
{
	const FVector* Attribute = VectorAttributes.Find(AttributeName);
	return Attribute ? *Attribute : FVector::ZeroVector;
}

float USecAttributeComponent::GetNumberAttribute(FName AttributeName) const
{
	const float* Attribute = NumberAttributes.Find(AttributeName);
	return Attribute ? *Attribute : 0.0f;
}

int32 USecAttributeComponent::GetValueAttribute(FName AttributeName) const
{
	const int32* Attribute = ValueAttributes.Find(AttributeName);
	return (Attribute ? *Attribute : 0);
}

UObject* USecAttributeComponent::GetObjectAttribute(FName AttributeName) const
{
	UObject*const* ObjectAttr = ObjectAttributes.Find(AttributeName);
	return ObjectAttr ? *ObjectAttr : nullptr;
}

bool USecAttributeComponent::DoesVectorAttributeExist(FName AttributeName) const
{
	return VectorAttributes.Contains(AttributeName);
}

bool USecAttributeComponent::DoesNumberAttributeExist(FName AttributeName) const
{
	return NumberAttributes.Contains(AttributeName);
}

bool USecAttributeComponent::DoesValueAttributeExist(FName AttributeName) const
{
	return ValueAttributes.Contains(AttributeName);
}

bool USecAttributeComponent::DoesObjectAttributeExist(FName AttributeName) const
{
	return ObjectAttributes.Contains(AttributeName);
}
/*
bool USecAttributeComponent::IsAction(FName AttributeName) const
{
	const bool bIsActionStateActive = GetActionAttribute(AttributeName);
	return bIsActionStateActive;
}

bool USecAttributeComponent::WasActionActive(FName AttributeName) const
{
	const bool bIsActionStateActive = GetActionAttribute(AttributeName);
	const bool bWasActionStateActive = Internal_GetActionAttribute(LastActionAttributes, AttributeName);
	return !bIsActionStateActive && bWasActionStateActive;
}

bool USecAttributeComponent::IsActionActivated(FName AttributeName) const
{
	const bool bIsActionStateActive = GetActionAttribute(AttributeName);
	const bool bWasActionStateActive = Internal_GetActionAttribute(LastActionAttributes, AttributeName);
	return bIsActionStateActive && !bWasActionStateActive;
}
*/
void USecAttributeComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	for (auto& It : ActionAttributes)
	{
		if (It.Value == ESecActionState::ActiveForOneFrame)
		{
			It.Value = ESecActionState::Inactive;
		}

		LastActionAttributes.FindOrAdd(It.Key) = It.Value;
	}
}

bool USecAttributeComponent::Internal_GetActionAttribute(const TMap<FName, ESecActionState>& InActionAttributes, FName AttributeName) const
{
	const ESecActionState* Attribute = InActionAttributes.Find(AttributeName);

	if (Attribute != nullptr && ((*Attribute) == ESecActionState::ActiveForOneFrame || (*Attribute) == ESecActionState::Active))
	{
		return true;
	}

	return false;
}

float USecAttributeComponent::ApplyNumberAttributeModifiers(FName AttributeName, float Attribute) const
{
	/*for (TSubclassOf<UAttributeModifier> AttributeModifierClass : AttributeModifiers)
	{
		UAttributeModifier* AttributeModifierCDO = AttributeModifierClass.GetDefaultObject();
		AttributeModifierCDO->ApplyModifiers(AttributeName, Attribute);
	}
	*/
	return Attribute;
}
