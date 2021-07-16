#pragma once

#include "Components/ActorComponent.h"
#include "secret/Attribute/SecActionState.h"
//#include "SecActionState.h"
#include "SecCapabilitySyncParams.h"
#include "SecScriptPriority.h"
#include "SecCapabilitySystemStatics.h"
#include "SecCapabilitySystemComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(CapabilitySystemLog, Log, All);

class USecCapability;
class USecCapabilitySheet;
class USecAttributeComponent;
class USecAttribute;

USTRUCT()
struct FSecCapabilityDeactivationCache
{
	GENERATED_BODY()
public:
	FCapabilityActivationParams ActivationParams;

	TWeakObjectPtr<USecCapability> CapabilityToActivate;

	int32 SyncTag = 0;
};

USTRUCT()
struct FSecCapabilityActivationCache
{
	GENERATED_BODY()
public:
	FCapabilityDeactivationParams DeactivationParams;

	TWeakObjectPtr<USecCapability> CapabilityToDeactivate;

	int32 SyncTag = 0;
};

USTRUCT()
struct FSecCapabilitySheetContainer
{
	GENERATED_BODY()
public:
	USecCapabilitySheet* Sheet = nullptr;

	ESecScriptPriority Priority;

	bool bApplied = false;
};

USTRUCT()
struct FSecReplicatedComponentContainer
{
	GENERATED_BODY()
public:
	UPROPERTY()
	TSubclassOf<UActorComponent> ComponentClass;

	UPROPERTY()
	FName ComponentName;

	UPROPERTY(NotReplicated)
	TWeakObjectPtr<UActorComponent> ComponentInstance;
};

USTRUCT()
struct FSecReplicatedCapabilitySheetData
{
	GENERATED_BODY()
public:
	UPROPERTY(Transient)
	USecCapabilitySheet* ReplicatedCapabilitySheet = nullptr;

	UPROPERTY()
	ESecScriptPriority Priority;

	bool bHasBeenApplied = false;
};

USTRUCT()
struct FSecReplicatedCapabilityData
{
	GENERATED_BODY()
public:
	UPROPERTY()
	TArray<FSecReplicatedComponentContainer> ReplicatedComponentContainers;

	UPROPERTY()
	TArray<FSecReplicatedCapabilitySheetData> ReplicatedCapabilitySheets;

	UPROPERTY()
	TArray<USecCapability*> ReplicatedCapabilities;
};

enum class ESecCapabilityCacheType : uint8
{
	Activate,
	Deactivate
};

USTRUCT()
struct FSecCapabilityCache
{
	GENERATED_BODY()
public:
	FSecSyncParams SyncParams;
	TWeakObjectPtr<USecCapability> Capability;
	float Elapsed = 0.0f;
	ESecCapabilityCacheType CacheType;
};

UCLASS()
class SECRET_API USecCapabilitySystemComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	USecCapabilitySystemComponent(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction);

	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

	void AddCapability(TSubclassOf<USecCapability> CapabilityClass);

	UFUNCTION(BlueprintCallable, Category = CapabilitySystem)
	void ApplySheet(USecCapabilitySheet* Sheet, ESecScriptPriority Priority = ESecScriptPriority::Script);

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	bool HasAuthority() const;

	bool IsLocallyControlled() const;

	UFUNCTION(BlueprintCallable, Category = CapabilitySystem)
	void BlockCapabilities(FName BlockingTag, UObject* Instigator);
	UFUNCTION(BlueprintCallable, Category = CapabilitySystem)
	void UnblockCapabilities(FName BlockingTag, UObject* Instigator);

	UPROPERTY(Transient)
	class USecAttributeComponent* AttributeComponent = nullptr;

	bool IsAction(FName ActionName) const;
	bool WasActionActivated(FName ActionName) const;
	bool IsActionActivated(FName ActionName) const;
	// Set action state to Inactive if currently Active or ActiveForOneFrame. Returns false if action state is already Inactive.
	bool ConsumeAction(FName ActionName);
	void SetActionState(FName ActionName, ESecActionState ActionState);

	bool DoesCapabilityExist(TSubclassOf<USecCapability> CapabilityClass) const;

protected:
	virtual void OnRegister() override;

private:

	// Attributes that are instantiated from a capability sheet.
	UPROPERTY(Transient)
	TArray<USecAttribute*> AttrributeInstances;

	// Components that are instantiated from a capability sheet.
	UPROPERTY(Transient)
	TArray<UActorComponent*> ComponentInstances;

	UPROPERTY(EditDefaultsOnly, Category = Capabilitys)
	TArray<TSubclassOf<USecCapability> > StartingCapabilities;

	TArray<FSecCapabilitySheetContainer> SheetContainers;

	TArray<FName> BlockingTags;

	TMap<FName, TArray<UObject*> > BlockingInstigators;

	struct FSecActionState
	{
		ESecActionState Previous = ESecActionState::Inactive;
		ESecActionState Current = ESecActionState::Inactive;
	};

	TMap<FName, FSecActionState> ActionAttributes;

	// List of all capabilities that could not be activated when received.
	TArray<FSecCapabilityCache> ClientCapabilityCache;

	UFUNCTION()
	void OnRep_CapabilitySheetContainer();

	UFUNCTION()
	void OnRep_ReplicatedCapabilities();

	UPROPERTY(Transient, ReplicatedUsing = OnRep_CapabilitySheetContainer)
	FSecCapabilitySheetSyncArray CapabilitySheetSyncContainer;

	UPROPERTY(ReplicatedUsing = OnRep_ReplicatedCapabilities)
	TArray<USecCapability*> ReplicatedCapabilities;

	UPROPERTY(Transient)
	TArray<USecCapability*> CapabilityCollection;

	int32 LocalSyncTag = 0;
	int32 RemoteSyncTag = 1;
};
