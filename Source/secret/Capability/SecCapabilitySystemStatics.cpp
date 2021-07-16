#include "SecCapabilitySystemStatics.h"
#include "SecCapability.h"
#include "SecCapabilitySystemComponent.h"

void UCapabilityActivationSyncParamsStatics::AddObject(FActivationSyncParams& ActivationSyncParams, FName Name, UObject* Object)
{
	ActivationSyncParams.EntriesObject.SetEntry(Name, Object);
}

void UCapabilityActivationSyncParamsStatics::AddVector(FActivationSyncParams& ActivationSyncParams, FName Name, const FVector& Vector)
{
	ActivationSyncParams.EntriesVector.SetEntry(Name, Vector);
}

void UCapabilityActivationSyncParamsStatics::AddNumber(FActivationSyncParams& ActivationSyncParams, FName Name, float Number)
{
	ActivationSyncParams.EntriesNumber.SetEntry(Name, Number);
}

void UCapabilityActivationSyncParamsStatics::AddValue(FActivationSyncParams& ActivationSyncParams, FName Name, int32 Value)
{
	ActivationSyncParams.EntriesValue.SetEntry(Name, Value);
}

void UCapabilityActivationSyncParamsStatics::AddActionState(FActivationSyncParams& ActivationSyncParams, FName Name)
{
	ActivationSyncParams.EntriesAction.SetEntry(Name);
}

UObject* UCapabilityActivationParamsStatics::GetObject(const FCapabilityActivationParams& ActivationParams, FName Name)
{
	return ActivationParams.EntriesObject.GetEntry(Name);
}

FVector UCapabilityActivationParamsStatics::GetVector(const FCapabilityActivationParams& ActivationParams, FName Name)
{
	FVector Vector = FVector::ZeroVector;
	ActivationParams.EntriesVector.GetEntry(Name, Vector);
	return Vector;
}

float UCapabilityActivationParamsStatics::GetNumber(const FCapabilityActivationParams& ActivationParams, FName Name)
{
	float Number = 0.0f;
	ActivationParams.EntriesNumber.GetEntry(Name, Number);
	return Number;
}

int32 UCapabilityActivationParamsStatics::GetValue(const FCapabilityActivationParams& ActivationParams, FName Name)
{
	int32 Value = 0;
	ActivationParams.EntriesValue.GetEntry(Name, Value);
	return Value;
}

bool UCapabilityActivationParamsStatics::GetActionState(const FCapabilityActivationParams& ActivationParams, FName Name)
{
	return ActivationParams.EntriesAction.EntriesAction.Contains(Name);
}

void UCapabilityDeactivationSyncParamsStatics::AddObject(FDeactivationSyncParams& DeactivationSyncParams, FName Name, UObject* Object)
{
	DeactivationSyncParams.EntriesObject.SetEntry(Name, Object);
}

void UCapabilityDeactivationSyncParamsStatics::AddVector(FDeactivationSyncParams& DeactivationSyncParams, FName Name, const FVector& Vector)
{
	DeactivationSyncParams.EntriesVector.SetEntry(Name, Vector);
}

void UCapabilityDeactivationSyncParamsStatics::AddNumber(FDeactivationSyncParams& DeactivationSyncParams, FName Name, float Number)
{
	DeactivationSyncParams.EntriesNumber.SetEntry(Name, Number);
}

void UCapabilityDeactivationSyncParamsStatics::AddValue(FDeactivationSyncParams& DeactivationSyncParams, FName Name, int32 Value)
{
	DeactivationSyncParams.EntriesValue.SetEntry(Name, Value);
}

void UCapabilityDeactivationSyncParamsStatics::AddActionState(FDeactivationSyncParams& DeactivationSyncParams, FName Name)
{
	DeactivationSyncParams.EntriesAction.SetEntry(Name);
}

UObject* UCapabilityDeactivationParamsStatics::GetObject(const FCapabilityDeactivationParams& DeactivationParams, FName Name)
{
	return DeactivationParams.EntriesObject.GetEntry(Name);
}

FVector UCapabilityDeactivationParamsStatics::GetVector(const FCapabilityDeactivationParams& DeactivationParams, FName Name)
{
	FVector Vector = FVector::ZeroVector;
	DeactivationParams.EntriesVector.GetEntry(Name, Vector);
	return Vector;
}

float UCapabilityDeactivationParamsStatics::GetNumber(const FCapabilityDeactivationParams& DeactivationParams, FName Name)
{
	float Number = 0.0f;
	DeactivationParams.EntriesNumber.GetEntry(Name, Number);
	return Number;
}

int32 UCapabilityDeactivationParamsStatics::GetValue(const FCapabilityDeactivationParams& DeactivationParams, FName Name)
{
	int32 Value = 0;
	DeactivationParams.EntriesValue.GetEntry(Name, Value);
	return Value;
}

bool UCapabilityDeactivationParamsStatics::GetActionState(const FCapabilityDeactivationParams& DeactivationParams, FName Name)
{
	return DeactivationParams.EntriesAction.EntriesAction.Contains(Name);
}

void USecActorCapabilitySystemStatics::AddCapability(AActor* Target, TSubclassOf<USecCapability> CapabilityClass)
{
	if (!ensureMsgf(CapabilityClass != nullptr, TEXT("Unable to add empty capability.")))
		return;

	if (!ensureMsgf(Target != nullptr, TEXT("Unable to add capability: %s to an empty target."), *CapabilityClass.GetDefaultObject()->GetName()))
		return;

	if (!Target->HasAuthority())
		return;

	USecCapabilitySystemComponent* CapabilitySystemComponent = Target->FindComponentByClass<USecCapabilitySystemComponent>();

	if (CapabilitySystemComponent == nullptr)
		CapabilitySystemComponent = CreateCapabilitySystem(Target);

	CapabilitySystemComponent->AddCapability(CapabilityClass);
}

void USecActorCapabilitySystemStatics::AddCapabilityByClassName(AActor* Target, FName CapabilityClassName, ESecAddCapabilityModule ModuleSearch)
{
	UClass* CapabilityClass = nullptr;

	if (ModuleSearch == ESecAddCapabilityModule::All)
	{
		CapabilityClass = FindObject<UClass>(nullptr, *CapabilityClassName.ToString());
		if (CapabilityClass == nullptr)
			CapabilityClass = FindObject<UClass>(nullptr, *(TEXT("/Script/secret.") + CapabilityClassName.ToString()));
		if (CapabilityClass == nullptr)
			CapabilityClass = FindObject<UClass>(nullptr, *(TEXT("/Script/Angelscript.") + CapabilityClassName.ToString()));
	}
	else if (ModuleSearch == ESecAddCapabilityModule::Angelscript)
	{
		CapabilityClass = FindObject<UClass>(nullptr, *(TEXT("/Script/Angelscript.") + CapabilityClassName.ToString()));
	}
	else if (ModuleSearch == ESecAddCapabilityModule::ProjectSecret)
	{
		CapabilityClass = FindObject<UClass>(nullptr, *(TEXT("/Script/secret.") + CapabilityClassName.ToString()));
	}
	else if (ModuleSearch == ESecAddCapabilityModule::Base)
	{
		CapabilityClass = FindObject<UClass>(nullptr, *CapabilityClassName.ToString());
	}

	if (CapabilityClass == nullptr)
	{
		// Debug print error
		return;
	}

	AddCapability(Target, CapabilityClass);
}

void USecActorCapabilitySystemStatics::SetActionState(AActor* TargetActor, FName ActionName, ESecActionState ActionState)
{
	if (!ensure(TargetActor != nullptr))
		return;

	USecCapabilitySystemComponent* CapabilitySystemComponent = TargetActor->FindComponentByClass<USecCapabilitySystemComponent>();

	if (!CapabilitySystemComponent)
		CapabilitySystemComponent = CreateCapabilitySystem(TargetActor);

	if (!ensure(CapabilitySystemComponent != nullptr))
		return;

	CapabilitySystemComponent->SetActionState(ActionName, ActionState);
}

USecCapabilitySystemComponent* USecActorCapabilitySystemStatics::CreateCapabilitySystem(AActor* Target)
{
	if (!ensure(Target != nullptr))
		return nullptr;
	USecCapabilitySystemComponent* CapabilitySystemComponent = NewObject<USecCapabilitySystemComponent>(Target, USecCapabilitySystemComponent::StaticClass());
	if (!ensureMsgf(CapabilitySystemComponent != nullptr, TEXT("unable to instantiate CapabilitySystemComponent for %s."), *Target->GetName()))
		return nullptr;
	CapabilitySystemComponent->RegisterComponent();
	return CapabilitySystemComponent;
}

FCapabilityActivationParams USecCapabilitySystemStatics::MakeActivationParamsFromSyncParams(const FActivationSyncParams& Params)
{
	FCapabilityActivationParams ActivationParams;

	for (const FSyncEntryNumber& EntryValue : Params.EntriesNumber.EntriesNumber)
		ActivationParams.EntriesNumber.SetEntry(EntryValue.ParamName, EntryValue.ParamNumber);

	for (const FSyncEntryVector& EntryVector : Params.EntriesVector.EntriesVector)
		ActivationParams.EntriesVector.SetEntry(EntryVector.ParamName, EntryVector.ParamVector);

	for (const FSyncEntryObject& EntryObject : Params.EntriesObject.EntriesObject)
		ActivationParams.EntriesObject.SetEntry(EntryObject.ParamName, EntryObject.ParamObject);

	for (const FSyncEntryValue& EntryValue : Params.EntriesValue.EntriesValue)
		ActivationParams.EntriesValue.SetEntry(EntryValue.ParamName, EntryValue.ParamValue);

	for (const FName ActionName : Params.EntriesAction.EntriesAction)
		ActivationParams.EntriesAction.EntriesAction.Add(ActionName);

	return ActivationParams;
}

FCapabilityDeactivationParams USecCapabilitySystemStatics::MakeDeactivationParamsFromSyncParams(const FDeactivationSyncParams& Params)
{
	FCapabilityDeactivationParams DeactivationParams;

	for (const FSyncEntryNumber& EntryValue : Params.EntriesNumber.EntriesNumber)
		DeactivationParams.EntriesNumber.SetEntry(EntryValue.ParamName, EntryValue.ParamNumber);

	for (const FSyncEntryVector& EntryVector : Params.EntriesVector.EntriesVector)
		DeactivationParams.EntriesVector.SetEntry(EntryVector.ParamName, EntryVector.ParamVector);

	for (const FSyncEntryObject& EntryObject : Params.EntriesObject.EntriesObject)
		DeactivationParams.EntriesObject.SetEntry(EntryObject.ParamName, EntryObject.ParamObject);

	for (const FSyncEntryValue& EntryValue : Params.EntriesValue.EntriesValue)
		DeactivationParams.EntriesValue.SetEntry(EntryValue.ParamName, EntryValue.ParamValue);

	for (const FName ActionName : Params.EntriesAction.EntriesAction)
		DeactivationParams.EntriesAction.EntriesAction.Add(ActionName);

	return DeactivationParams;
}

FActivationSyncParams USecCapabilitySystemStatics::MakeActivationSyncParamsFromActivationParams(const FCapabilityActivationParams& Params)
{
	FActivationSyncParams SyncParams;

	for (const FSyncEntryNumber& EntryValue : Params.EntriesNumber.EntriesNumber)
		SyncParams.EntriesNumber.SetEntry(EntryValue.ParamName, EntryValue.ParamNumber);

	for (const FSyncEntryVector& EntryVector : Params.EntriesVector.EntriesVector)
		SyncParams.EntriesVector.SetEntry(EntryVector.ParamName, EntryVector.ParamVector);

	for (const FSyncEntryObject& EntryObject : Params.EntriesObject.EntriesObject)
		SyncParams.EntriesObject.SetEntry(EntryObject.ParamName, EntryObject.ParamObject);

	for (const FSyncEntryValue& EntryValue : Params.EntriesValue.EntriesValue)
		SyncParams.EntriesValue.SetEntry(EntryValue.ParamName, EntryValue.ParamValue);

	for (const FName ActionName : Params.EntriesAction.EntriesAction)
		SyncParams.EntriesAction.EntriesAction.Add(ActionName);

	return SyncParams;
}

FDeactivationSyncParams USecCapabilitySystemStatics::MakeDeactivationSyncParamsFromDeactivationParams(const FCapabilityDeactivationParams& Params)
{
	FDeactivationSyncParams SyncParams;

	for (const FSyncEntryNumber& EntryValue : Params.EntriesNumber.EntriesNumber)
		SyncParams.EntriesNumber.SetEntry(EntryValue.ParamName, EntryValue.ParamNumber);

	for (const FSyncEntryVector& EntryVector : Params.EntriesVector.EntriesVector)
		SyncParams.EntriesVector.SetEntry(EntryVector.ParamName, EntryVector.ParamVector);

	for (const FSyncEntryObject& EntryObject : Params.EntriesObject.EntriesObject)
		SyncParams.EntriesObject.SetEntry(EntryObject.ParamName, EntryObject.ParamObject);

	for (const FSyncEntryValue& EntryValue : Params.EntriesValue.EntriesValue)
		SyncParams.EntriesValue.SetEntry(EntryValue.ParamName, EntryValue.ParamValue);

	for (const FName ActionName : Params.EntriesAction.EntriesAction)
		SyncParams.EntriesAction.EntriesAction.Add(ActionName);

	return SyncParams;
}
