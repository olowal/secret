#pragma once

#include "Network/SecNetObject.h"
#include "Engine/NetSerialization.h"
#include "SecScriptPriority.h"
#include "SecCapabilitySyncParams.generated.h"

USTRUCT()
struct FSyncEntryNumber
{
	GENERATED_BODY()
public:
	UPROPERTY()
	FName ParamName;
	UPROPERTY()
	float ParamNumber;
};

USTRUCT()
struct FSyncEntryValue
{
	GENERATED_BODY()
public:
	UPROPERTY()
	FName ParamName;
	UPROPERTY()
	int32 ParamValue;
};

USTRUCT()
struct FSyncEntryVector
{
	GENERATED_BODY()
public:
	UPROPERTY()
	FVector ParamVector;
	UPROPERTY()
	FName ParamName;
};

USTRUCT()
struct FSyncEntryObject
{
	GENERATED_BODY()
public:
	UPROPERTY()
	UObject* ParamObject;
	UPROPERTY()
	FName ParamName;
};

USTRUCT(BlueprintType)
struct FSyncEntriesVector
{
	GENERATED_BODY()
public:
	UPROPERTY()
	TArray<FSyncEntryVector> EntriesVector;

	bool GetEntry(FName EntryName, FVector& Entry) const
	{
		for (const FSyncEntryVector& EntryVector : EntriesVector)
		{
			if (EntryVector.ParamName == EntryName)
			{
				Entry = EntryVector.ParamVector;
				return true;
			}
		}

		return false;
	}

	void SetEntry(FName EntryName, const FVector& Entry)
	{
		bool bAdded = false;

		for (FSyncEntryVector& EntryVector : EntriesVector)
		{
			if (EntryVector.ParamName == EntryName)
			{
				EntryVector.ParamVector = Entry;
				bAdded = true;
				break;
			}
		}

		if (!bAdded)
		{
			FSyncEntryVector NewEntry;
			NewEntry.ParamName = EntryName;
			NewEntry.ParamVector = Entry;
			EntriesVector.Add(NewEntry);
		}
	}

	int32 Num() const
	{
		return EntriesVector.Num();
	}
};

USTRUCT(BlueprintType)
struct FSyncEntriesNumber
{
	GENERATED_BODY()
public:
	UPROPERTY()
	TArray<FSyncEntryNumber> EntriesNumber;

	bool GetEntry(FName EntryName, float& Entry) const
	{
		for (const FSyncEntryNumber& EntryValue: EntriesNumber)
		{
			if (EntryValue.ParamName == EntryName)
			{
				Entry = EntryValue.ParamNumber;
				return true;
			}
		}

		return false;
	}

	void SetEntry(FName EntryName, const float& Entry)
	{
		bool bAdded = false;

		for (FSyncEntryNumber& EntryValue : EntriesNumber)
		{
			if (EntryValue.ParamName == EntryName)
			{
				EntryValue.ParamNumber = Entry;
				bAdded = true;
				break;
			}
		}

		if (!bAdded)
		{
			FSyncEntryNumber NewEntry;
			NewEntry.ParamName = EntryName;
			NewEntry.ParamNumber= Entry;
			EntriesNumber.Add(NewEntry);
		}
	}

	int32 Num() const
	{
		return EntriesNumber.Num();
	}
};

USTRUCT(BlueprintType)
struct FSyncEntriesValue
{
	GENERATED_BODY()
public:
	UPROPERTY()
	TArray<FSyncEntryValue> EntriesValue;

	bool GetEntry(FName EntryName, int32& Entry) const
	{
		for (const FSyncEntryValue& EntryValue : EntriesValue)
		{
			if (EntryValue.ParamName == EntryName)
			{
				Entry = EntryValue.ParamValue;
				return true;
			}
		}

		return false;
	}

	void SetEntry(FName EntryName, const float& Entry)
	{
		bool bAdded = false;

		for (FSyncEntryValue& EntryValue : EntriesValue)
		{
			if (EntryValue.ParamName == EntryName)
			{
				EntryValue.ParamValue = Entry;
				bAdded = true;
				break;
			}
		}

		if (!bAdded)
		{
			FSyncEntryValue NewEntry;
			NewEntry.ParamName = EntryName;
			NewEntry.ParamValue = Entry;
			EntriesValue.Add(NewEntry);
		}
	}

	int32 Num() const
	{
		return EntriesValue.Num();
	}
};

USTRUCT(BlueprintType)
struct FSyncEntriesObject
{
	GENERATED_BODY()
public:
	UPROPERTY()
	TArray<FSyncEntryObject> EntriesObject;

	UObject* GetEntry(FName EntryName) const
	{
		for (const FSyncEntryObject& EntryValue: EntriesObject)
		{
			if (EntryValue.ParamName == EntryName)
			{
				return EntryValue.ParamObject;
			}
		}

		return nullptr;
	}

	void SetEntry(FName EntryName, UObject* EntryObject)
	{
		bool bAdded = false;

		for (FSyncEntryObject& EntryValue : EntriesObject)
		{
			if (EntryValue.ParamName == EntryName)
			{
				EntryValue.ParamObject = EntryObject;
				bAdded = true;
				break;
			}
		}

		if (!bAdded)
		{
			FSyncEntryObject NewEntry;
			NewEntry.ParamName = EntryName;
			NewEntry.ParamObject= EntryObject;
			EntriesObject.Add(NewEntry);
		}
	}

	int32 Num() const
	{
		return EntriesObject.Num();
	}
};

USTRUCT()
struct FSyncEntryAction
{
	GENERATED_BODY()
public:
	UPROPERTY()
	TArray<FName> EntriesAction;

	void SetEntry(FName Entry)
	{
		if (!EntriesAction.Contains(Entry))
		{
			EntriesAction.Add(Entry);
		}
	}

	bool GetEntry(FName Entry) const
	{
		return EntriesAction.Contains(Entry);
	}

	int32 Num() const { return EntriesAction.Num(); }
};

USTRUCT()
struct FSecSyncParams
{
	GENERATED_BODY()
public:
	UPROPERTY()
	FSyncEntriesVector EntriesVector;
	UPROPERTY()
	FSyncEntriesNumber EntriesNumber;
	UPROPERTY()
	FSyncEntriesValue EntriesValue;
	UPROPERTY()
	FSyncEntriesObject EntriesObject;
	UPROPERTY()
	FSyncEntryAction EntriesAction;
	UPROPERTY()
	int32 SyncTag = 0;
	UPROPERTY()
	bool bPrediction = false;
};

USTRUCT(BlueprintType)
struct FActivationSyncParams : public FSecSyncParams
{
	GENERATED_BODY()
public:
	FActivationSyncParams(){}

	FActivationSyncParams(const FSecSyncParams& InSyncParams)
	{
		EntriesVector = InSyncParams.EntriesVector;
		EntriesNumber = InSyncParams.EntriesNumber;
		EntriesValue = InSyncParams.EntriesValue;
		EntriesObject = InSyncParams.EntriesObject;
		EntriesAction = InSyncParams.EntriesAction;
		SyncTag = InSyncParams.SyncTag;
		bPrediction = InSyncParams.bPrediction;
	}
};

USTRUCT(BlueprintType)
struct FDeactivationSyncParams : public FSecSyncParams
{
	GENERATED_BODY()
public:
	FDeactivationSyncParams(){}
	FDeactivationSyncParams(const FSecSyncParams& InSyncParams)
	{
		EntriesVector = InSyncParams.EntriesVector;
		EntriesNumber = InSyncParams.EntriesNumber;
		EntriesValue = InSyncParams.EntriesValue;
		EntriesObject = InSyncParams.EntriesObject;
		EntriesAction = InSyncParams.EntriesAction;
		SyncTag = InSyncParams.SyncTag;
		bPrediction = InSyncParams.bPrediction;
	}
};

UENUM()
enum class ESecCapabilitySyncType : uint8
{
	Activate,
	Deactivate
};

USTRUCT()
struct FSecReplicatedCapabilityCrumb : public FFastArraySerializerItem
{
	GENERATED_BODY()
public:
	FSecReplicatedCapabilityCrumb(){}
	FSecReplicatedCapabilityCrumb(FSecSyncParams InSyncParams, ESecCapabilitySyncType InSyncType) :
		SyncParams(InSyncParams),
		SyncType(InSyncType) {}

	UPROPERTY()
	FSecSyncParams SyncParams;

	UPROPERTY()
	ESecCapabilitySyncType SyncType = ESecCapabilitySyncType::Activate;

	float TimeUntilRemoval = 0.0f;
};

USTRUCT()
struct FSecCapabilityCrumb
{
	GENERATED_BODY()
public:
	FSecCapabilityCrumb(){}
	FSecCapabilityCrumb(const FSecReplicatedCapabilityCrumb& ReplicatedCrumb) :
		SyncParams(ReplicatedCrumb.SyncParams),
		SyncType(ReplicatedCrumb.SyncType) {}

	bool operator < (const FSecCapabilityCrumb& InCrumb) const { return SyncParams.SyncTag < InCrumb.SyncParams.SyncTag; }
	bool operator <= (const FSecCapabilityCrumb& InCrumb) const { return SyncParams.SyncTag <= InCrumb.SyncParams.SyncTag; }
	bool operator > (const FSecCapabilityCrumb& InCrumb) const { return SyncParams.SyncTag > InCrumb.SyncParams.SyncTag; }
	bool operator >= (const FSecCapabilityCrumb& InCrumb) const { return SyncParams.SyncTag >= InCrumb.SyncParams.SyncTag; }
	bool operator == (const FSecReplicatedCapabilityCrumb& InCrumb) const { return SyncParams.SyncTag == InCrumb.SyncParams.SyncTag; }

	FSecSyncParams SyncParams;
	ESecCapabilitySyncType SyncType = ESecCapabilitySyncType::Activate;
};

USTRUCT()
struct FSecCapabilityCrumbList : public FFastArraySerializer
{
	GENERATED_BODY()
public:

	UPROPERTY()
	TArray<FSecReplicatedCapabilityCrumb>	Items;

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FSecReplicatedCapabilityCrumb, FSecCapabilityCrumbList>(Items, DeltaParms, *this);
	}
};

template<>
struct TStructOpsTypeTraits< FSecCapabilityCrumbList > : public TStructOpsTypeTraitsBase2< FSecCapabilityCrumbList >
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};

class USecCapabilitySheet;
class USecCapability;

UENUM()
enum class ESecCapabilitySheetSyncState : uint8
{
	Apply,
	Remove
};

USTRUCT()
struct FSecCapabilitySheetSyncEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()
public:
	UPROPERTY(Transient, NotReplicated)
	TArray<UActorComponent*> ComponentInstances;

	UPROPERTY()
	TArray<USecCapability*> CapabilityCollection;

	UPROPERTY()
	USecCapabilitySheet* CapabilitySheet = nullptr;

	UPROPERTY()
	ESecScriptPriority Priority = ESecScriptPriority::Game;

	UPROPERTY()
	ESecCapabilitySheetSyncState SyncState = ESecCapabilitySheetSyncState::Apply;
	
	bool bClientApplied = false;
};

USTRUCT()
struct FSecCapabilitySheetSyncArray : public FFastArraySerializer
{
	GENERATED_BODY()
public:

	UPROPERTY()
	TArray<FSecCapabilitySheetSyncEntry> Items;

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FSecCapabilitySheetSyncEntry, FSecCapabilitySheetSyncArray>(Items, DeltaParms, *this);
	}
};

template<>
struct TStructOpsTypeTraits< FSecCapabilitySheetSyncArray > : public TStructOpsTypeTraitsBase2< FSecCapabilitySheetSyncArray >
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};
