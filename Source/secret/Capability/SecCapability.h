#pragma once

#include "Network/SecNetObject.h"
#include "SecCapabilitySystemStatics.h"
#include "SecCapability.generated.h"

class USecAttributeComponent;

// State to specify what the capability is looking for in terms of activation steps
enum class ELocalPredictionWithAuthorityState : uint8
{
	Inactive,	// Nothing
	AwaitingAuthority,	// Capability has activated locally and is awaiting response from authority to continue
	Done,
	TimedOut	// No response has arrived from authority. This will trigger a rollback function call.
};

UCLASS()
class SECRET_API USecCapability : public USecNetObject
{
	GENERATED_BODY()
public:

	void Setup();
	UFUNCTION(BlueprintImplementableEvent, Category = Capability, meta = (DisplayName = "Setup"))
	void BP_Setup();

	virtual class UWorld* GetWorld() const override;

	virtual bool IsSupportedForNetworking() const override;

	UFUNCTION(BlueprintCallable, Category = Capability)
	void SetDefaultIsNetworked(bool bValue);

	UFUNCTION(BlueprintCallable, Category = Capability)
	void SetDefaultRequiresController(bool bValue);

	void PreActivation(FActivationSyncParams& SyncParams);
	void PreDeactivation(FDeactivationSyncParams& SyncParams);

	UFUNCTION(BlueprintImplementableEvent, Category = Capability, meta = (DisplayName = "PreActivation"))
	void BP_PreActivation(FActivationSyncParams& SyncParams);
	UFUNCTION(BlueprintImplementableEvent, Category = Capability, meta = (DisplayName = "PreDeactivation"))
	void BP_PreDeactivation(FDeactivationSyncParams& SyncParams);

	bool IsBlockedByTags(const TArray<FName>& BlockingTags) const;

	ECapabilityActivation ShouldActivate(const TArray<FName>& BlockingTags, const int32 InRemoteSyncTag) const;

	UFUNCTION(BlueprintNativeEvent, Category = Capability, meta = (DisplayName = "Should Activate"))
	ECapabilityActivation BP_ShouldActivate() const;

	bool RemoteShouldActivate() const;

	UFUNCTION(BlueprintNativeEvent, Category = Capability, meta = (DisplayName = "Remote Should Activate"))
	bool BP_RemoteShouldActivate() const;

	void HandleFailedActivationPrediction();

	UFUNCTION(BlueprintImplementableEvent, Category = Capability, meta = (DisplayName = "Handle Failed Activation Prediction"))
	void BP_HandleFailedActivationPrediction();

	ECapabilityDeactivation ShouldDeactivate(const TArray<FName>& BlockingTags, const int32 InRemoteSyncTag) const;

	UFUNCTION(BlueprintNativeEvent, Category = Capability, meta = (DisplayName = "Should Deactivate"))
	ECapabilityDeactivation BP_ShouldDeactivate() const;

	void HandleFailedDeactivationPrediction();

	UFUNCTION(BlueprintImplementableEvent, Category = Capability, meta = (DisplayName = "Handle Failed Deactivation Prediction"))
	void BP_HandleFailedDeactivationPrediction();

	EServerCapabilityActivation OnServer_ConfirmActivation(const FCapabilityActivationParams& ActivationParams, FActivationSyncParams& SyncParams);
	EServerCapabilityDeactivation OnServer_ConfirmDeactivation(const FCapabilityDeactivationParams& DeactivationParams, FDeactivationSyncParams& SyncParams);

	UFUNCTION(BlueprintNativeEvent, Category = Capability, meta = (DisplayName = "Server Confirm Activation", ScriptName = "Server_ConfirmActivation"))
	EServerCapabilityActivation BP_OnServer_ConfirmActivation(const FCapabilityActivationParams& ActivationParams, FActivationSyncParams& SyncParams);

	UFUNCTION(BlueprintNativeEvent, Category = Capability, meta = (DisplayName = "Server Confirm Deactivation", ScriptName = "Server_ConfirmDeactivation"))
	EServerCapabilityDeactivation BP_OnServer_ConfirmDeactivation(const FCapabilityDeactivationParams& DeactivationParams, FDeactivationSyncParams& SyncParams);
	
	void OnActivate(const FActivationSyncParams& SyncParams);

	UFUNCTION(BlueprintImplementableEvent, Category = Capability, meta = (DisplayName = "On Activate"))
	void BP_OnActivate(const FCapabilityActivationParams& ActivationParams);

	void OnDeactivate(const FDeactivationSyncParams& SyncParams);

	UFUNCTION(BlueprintImplementableEvent, Category = Capability, meta = (DisplayName = "On Deactivate"))
	void BP_OnDeactivate(const FCapabilityDeactivationParams& DeactivationParams);

	void TickActive(float DeltaTime);
	void PreTick(float DeltaTime);

	UFUNCTION(BlueprintImplementableEvent, Category = Capability, meta = (DisplayName = "Tick Active"))
	void BP_TickActive(float DeltaTime);

	UFUNCTION(BlueprintImplementableEvent, Category = Capability, meta = (DisplayName = "Pre Tick"))
	void BP_PreTick(float DeltaTime);

	void ConsumeCrumbs(int32& InSyncTag);

	UFUNCTION(BlueprintCallable, Category = Capability)
	void BlockCapabilities(FName BlockingTag, UObject* Instigator);

	UFUNCTION(BlueprintCallable, Category = Capability)
	void UnblockCapabilities(FName BlockingTag, UObject* Instigator);

	UFUNCTION(BlueprintPure, Category = Capability)
	bool IsCapabilityActive() const;

	UFUNCTION(BlueprintPure, Category = Capability)
	bool IsLocallyControlled() const;

	UFUNCTION(BlueprintPure, Category = Capability)
	class USecCapabilitySystemComponent* GetCapabilitySystemComponent() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tags)
	TArray<FName> Tags;

	UPROPERTY(Transient)
	class USecCapabilitySystemComponent* CapabilitySystem = nullptr;

	UFUNCTION(BlueprintPure, Category = Capability)
	class AActor* GetOwner() const;

	UFUNCTION(BlueprintPure, Category = Capability)
	ENetRole GetLocalRole() const;

	UFUNCTION(BlueprintPure, Category = Capability)
	ENetRole GetRemoteRole() const;

	UFUNCTION(BlueprintPure, Category = Capability)
	bool HasAuthority() const;

	UPROPERTY(Transient)
	class AActor* Owner = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Tick)
	int32 TickOrder = 0;

	UFUNCTION(BlueprintPure, Category = Capability)
	USecAttributeComponent* GetAttribute() const;

	UFUNCTION(BlueprintPure, Category = "Capability")
	float GetTimeActive() const { return TimeActive; }
	UFUNCTION(BlueprintPure, Category = "Capability")
	float GetTimeActiveTotal() const { return TimeActiveTotal; }

	UFUNCTION(BlueprintPure, Category = Capability)
	bool IsAction(FName ActionName) const;
	UFUNCTION(BlueprintPure, Category = Capability)
	bool WasActionActivated(FName ActionName) const;
	UFUNCTION(BlueprintPure, Category = Capability)
	bool IsActionActivated(FName ActionName) const;
	// Set action state to Inactive if currently Active or ActiveForOneFrame. Returns false if action state is already Inactive.
	UFUNCTION(BlueprintCallable, Category = Capability)
	bool ConsumeAction(FName ActionName);
	UFUNCTION(BlueprintCallable, Category = Capability)
	void SetActionState(FName ActionName, ESecActionState ActionState);

	bool ShouldTickActive() const { return bIsCapabilityActive; }
	bool ShouldCheckActivation() const { return !bIsCapabilityActive; }
	bool ShouldCheckDeactivation() const { return bIsCapabilityActive; }

	void NetworkActivation(int32 InSyncTag);
	void NetworkDeactivation(int32 InSyncTag);

	void ServerOnlyActivation();

	void RemoteActivation(int32 InRemoteSyncTag);
	void RemoteDeactivation(int32 InRemoteSyncTag);

	int32 GetSyncTag() const { return SyncTag; }

	FString GetCapabilityLogString(const FString& LogString) const;
	void LogCapabilityDisplay(const FString& LogString) const;
	void LogCapabilityWarning(const FString& LogString) const;

	FString GetLocalRoleAsString() const;

	bool HasRunSetup() const { return bHasRunSetup; }

	UFUNCTION(BlueprintCallable, Category = Capability)
	int32 TestFunction(float Val, float Max);

	UFUNCTION(BlueprintPure, Category = Capability)
	bool HasAnyCrumbs() const { return GetNumCrumbs() != 0; }

	UFUNCTION(BlueprintPure, Category = Capability)
	int32 GetNumCrumbs() const { return CrumbList.Num(); }

private:

	UFUNCTION(Server, Reliable)
	void Server_Activate(FActivationSyncParams SyncParams);
	UFUNCTION(Server, Reliable)
	void Server_Deactivate(FDeactivationSyncParams SyncParams);

	UFUNCTION(Server, Reliable)
	void ServerOnly_Activate(FActivationSyncParams SyncParams);

	void AddReplicatedCrumb(FSecReplicatedCapabilityCrumb InCrumb);

	UFUNCTION()
	void OnRep_ReplicatedCrumbs();

	UPROPERTY(ReplicatedUsing = OnRep_ReplicatedCrumbs)
	FSecCapabilityCrumbList ReplicatedCrumbs;

	// This list should always be sorted so that the 
	TArray<FSecCapabilityCrumb> CrumbList;

	int32 SyncTag = 0;
	int32 LastRemoteSyncTag = 0;

	// Total time spent active.
	float TimeActive = 0.0f;
	// Total time active since instantiated.
	float TimeActiveTotal = 0.0f;

	bool bIsCapabilityActive = false;
	bool bHasRunSetup = false;
	bool bIsCapabilityNetworked = true;
	// Setting this to true will prevent Setup from running until the owner is possessed by any controller.
	bool bRequiresController = false;
};
