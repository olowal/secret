#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SecCapabilitySyncParams.h"
#include "Attribute/SecActionState.h"
#include "SecCapabilitySystemStatics.generated.h"

class USecCapability;
class USecCapabilitySystemComponent;

UENUM(BlueprintType)
enum class ECapabilityActivation : uint8
{
	// Any role will activate, no network replication is performed.
	Local,
	// Replicate activation on all clients
	Network,
	// Runs OnActivate locally and Server_ConfirmActivation.
	Server,
	// Don't activate this capability.
	DontActivate,
	// Only for internal use, not exposed
	RemoteActivate UMETA(Hidden)
};

UENUM(BlueprintType)
enum class ECapabilityDeactivation : uint8
{
	// Any role will deactivate, no network replication is performed.
	Local,
	// Authority or locally controlled will deactivate and then replicate to clients. Runs PreDeactivate before Server_OnDeactivate and OnDeactivate.
	Network,
	// Don't deactivate this capability.
	DontDeactivate,
	// Only for internal use, not exposed
	RemoteDeactivate UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EServerCapabilityActivation : uint8
{
	// Everything is fine, continue with activation
	Activate,
	// Continue with activation but attempt to modify locally 
	CorrectClient,
	DontActivate,
	Disconnect
};

UENUM(BlueprintType)
enum class EServerCapabilityDeactivation : uint8
{
	Deactivate,
	CorrectClient,
	DontDeactivate,
	Disconnect
};

USTRUCT(BlueprintType)
struct FCapabilityActivationParams
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
};

USTRUCT(BlueprintType)
struct FCapabilityDeactivationParams
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
};

UCLASS(meta = (ScriptMixin = "FActivationSyncParams"))
class SECRET_API UCapabilityActivationSyncParamsStatics : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(ScriptCallable)
	static void AddObject(FActivationSyncParams& ActivationSyncParams, FName Name, UObject* Object);
	UFUNCTION(ScriptCallable)
	static void AddVector(FActivationSyncParams& ActivationSyncParams, FName Name, const FVector& Vector);
	UFUNCTION(ScriptCallable)
	static void AddNumber(FActivationSyncParams& ActivationSyncParams, FName Name, float Number);
	UFUNCTION(ScriptCallable)
	static void AddValue(FActivationSyncParams& ActivationSyncParams, FName Name, int32 Value);
	UFUNCTION(ScriptCallable)
	static void AddActionState(FActivationSyncParams& ActivationSyncParams, FName Name);
};

UCLASS(meta = (ScriptMixin = "FCapabilityActivationParams"))
class SECRET_API UCapabilityActivationParamsStatics : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(ScriptCallable)
	static UObject* GetObject(const FCapabilityActivationParams& ActivationParams, FName Name);
	UFUNCTION(ScriptCallable)
	static FVector GetVector(const FCapabilityActivationParams& ActivationParams, FName Name);
	UFUNCTION(ScriptCallable)
	static float GetNumber(const FCapabilityActivationParams& ActivationParams, FName Name);
	UFUNCTION(ScriptCallable)
	static int32 GetValue(const FCapabilityActivationParams& ActivationParams, FName Name);
	UFUNCTION(ScriptCallable)
	static bool GetActionState(const FCapabilityActivationParams& ActivationParams, FName Name);
};

UCLASS(meta = (ScriptMixin = "FDeactivationSyncParams"))
class SECRET_API UCapabilityDeactivationSyncParamsStatics : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(ScriptCallable)
	static void AddObject(FDeactivationSyncParams& DeactivationSyncParams, FName Name, UObject* Object);
	UFUNCTION(ScriptCallable)
	static void AddVector(FDeactivationSyncParams& DeactivationSyncParams, FName Name, const FVector& Vector);
	UFUNCTION(ScriptCallable)
	static void AddNumber(FDeactivationSyncParams& DeactivationSyncParams, FName Name, float Number);
	UFUNCTION(ScriptCallable)
	static void AddValue(FDeactivationSyncParams& DeactivationSyncParams, FName Name, int32 Value);
	UFUNCTION(ScriptCallable)
	static void AddActionState(FDeactivationSyncParams& DeactivationSyncParams, FName Name);
};

UCLASS(meta = (ScriptMixin = "FCapabilityDeactivationParams"))
class SECRET_API UCapabilityDeactivationParamsStatics : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(ScriptCallable)
	static UObject* GetObject(const FCapabilityDeactivationParams& DeactivationParams, FName Name);
	UFUNCTION(ScriptCallable)
	static FVector GetVector(const FCapabilityDeactivationParams& DeactivationParams, FName Name);
	UFUNCTION(ScriptCallable)
	static float GetNumber(const FCapabilityDeactivationParams& DeactivationParams, FName Name);
	UFUNCTION(ScriptCallable)
	static int32 GetValue(const FCapabilityDeactivationParams& DeactivationParams, FName Name);
	UFUNCTION(ScriptCallable)
	static bool GetActionState(const FCapabilityDeactivationParams& DeactivationParams, FName Name);
};

UENUM(BlueprintType)
enum class ESecAddCapabilityModule : uint8
{
	All,
	ProjectSecret,
	Angelscript,
	Base
};

UCLASS(meta = (ScriptMixin = "AActor"))
class SECRET_API USecActorCapabilitySystemStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = Capability)
	static void AddCapability(AActor* Target, TSubclassOf<USecCapability> CapabilityClass);

	UFUNCTION(ScriptCallable, Category = Capability, meta = (ScriptName = "AddCapability"))
	static void AddCapabilityByClassName(AActor* Target, FName CapabilityClassName, ESecAddCapabilityModule ModuleSearch = ESecAddCapabilityModule::Angelscript);

	UFUNCTION(ScriptCallable, Category = Capability)
	static void SetActionState(AActor* TargetActor, FName ActionName, ESecActionState ActionState);

private:
	static USecCapabilitySystemComponent* CreateCapabilitySystem(AActor* Target);
};

UCLASS()
class SECRET_API USecCapabilitySystemStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	static FCapabilityActivationParams MakeActivationParamsFromSyncParams(const FActivationSyncParams& Params);
	static FCapabilityDeactivationParams MakeDeactivationParamsFromSyncParams(const FDeactivationSyncParams& Params);

	static FActivationSyncParams MakeActivationSyncParamsFromActivationParams(const FCapabilityActivationParams& Params);
	static FDeactivationSyncParams MakeDeactivationSyncParamsFromDeactivationParams(const FCapabilityDeactivationParams& Params);
};
