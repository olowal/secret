#include "SecCapability.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Net/UnrealNetwork.h"
#include "SecCapabilitySystemComponent.h"
#include "SecUtilityStatics.h"
#include "Math/SecMath.h"

DEFINE_LOG_CATEGORY(CapabilitySystemLog);

void USecCapability::Setup()
{
	if (!bHasRunSetup)
	{
		BP_Setup();
		bHasRunSetup = true;
	}
}

UWorld* USecCapability::GetWorld() const
{
	return GetOuter()->GetWorld();
}

bool USecCapability::IsSupportedForNetworking() const
{
	return bIsCapabilityNetworked;
}

void USecCapability::SetDefaultIsNetworked(bool bValue)
{
	if (ensureMsgf(bHasRunSetup, TEXT("Cannot change network behavior on %s, after Setup has run. Set this value in the constructor instead."), *GetName()))
		return;
	bIsCapabilityNetworked = bValue;
}

void USecCapability::SetDefaultRequiresController(bool bValue)
{
	if (ensureMsgf(bHasRunSetup, TEXT("Cannot change Controller requirements on %s, after Setup has run. Set this value in the constructor instead."), *GetName()))
		return;
	bRequiresController = bValue;
}

void USecCapability::PreActivation(FActivationSyncParams& SyncParams)
{
	BP_PreActivation(SyncParams);
}

void USecCapability::PreDeactivation(FDeactivationSyncParams& SyncParams)
{
	BP_PreDeactivation(SyncParams);
}

bool USecCapability::IsBlockedByTags(const TArray<FName>& InBlockingTags) const
{
	for (FName BlockingTag : InBlockingTags)
	{
		if (Tags.Contains(BlockingTag))
		{
			return true;
		}
	}

	return false;
}

ECapabilityActivation USecCapability::ShouldActivate(const TArray<FName>& BlockingTags, const int32 InRemoteSyncTag) const
{
	if (bIsCapabilityActive || IsBlockedByTags(BlockingTags))
	{
		return ECapabilityActivation::DontActivate;
	}

	ECapabilityActivation ActivationResult = BP_ShouldActivate();

	// Temp hack since RemoteActivate is available in AS.
	if (ActivationResult == ECapabilityActivation::RemoteActivate)
		ActivationResult = ECapabilityActivation::DontActivate;

	if (!IsLocallyControlled() && ActivationResult != ECapabilityActivation::Local)
	{
		if (HasAnyCrumbs() && RemoteShouldActivate())
		{
			if (CrumbList.Last().SyncParams.SyncTag == InRemoteSyncTag)
			{
				return ECapabilityActivation::RemoteActivate;
			}
		}
	}

	return ActivationResult;
}

bool USecCapability::RemoteShouldActivate() const
{
	return BP_RemoteShouldActivate();
}

bool USecCapability::BP_RemoteShouldActivate_Implementation() const
{
	return true;
}

void USecCapability::HandleFailedActivationPrediction()
{
	BP_HandleFailedActivationPrediction();
}

ECapabilityDeactivation USecCapability::ShouldDeactivate(const TArray<FName>& BlockingTags, const int32 InRemoteSyncTag) const
{
	if (IsBlockedByTags(BlockingTags))
	{
		return ECapabilityDeactivation::Local;
	}

	ECapabilityDeactivation DeactivationResult = BP_ShouldDeactivate();

	if (DeactivationResult == ECapabilityDeactivation::RemoteDeactivate)
		DeactivationResult = ECapabilityDeactivation::DontDeactivate;

	if (!IsLocallyControlled() && DeactivationResult != ECapabilityDeactivation::Local)
	{
		if (HasAnyCrumbs())
		{
			if (CrumbList.Last().SyncParams.SyncTag == InRemoteSyncTag)
			{
				return ECapabilityDeactivation::RemoteDeactivate;
			}
		}
	}

	return DeactivationResult;
}

void USecCapability::HandleFailedDeactivationPrediction()
{
	BP_HandleFailedDeactivationPrediction();
}

EServerCapabilityActivation USecCapability::OnServer_ConfirmActivation(const FCapabilityActivationParams& ActivationParams, FActivationSyncParams& SyncParams)
{
	if (!HasAuthority())
		return EServerCapabilityActivation::DontActivate;
	
	return BP_OnServer_ConfirmActivation(ActivationParams, SyncParams);
}

EServerCapabilityActivation USecCapability::BP_OnServer_ConfirmActivation_Implementation(const FCapabilityActivationParams& ActivationParams, FActivationSyncParams& SyncParams)
{
	return EServerCapabilityActivation::Activate;
}

void USecCapability::OnActivate(const FActivationSyncParams& SyncParams)
{
	FCapabilityActivationParams ActivationParams = USecCapabilitySystemStatics::MakeActivationParamsFromSyncParams(SyncParams);
	if (!ensureMsgf(!bIsCapabilityActive, TEXT("Attempting to activate %s, but this capability is already active!"), *GetName()))
	{
		LogCapabilityWarning(TEXT("Already active, unable to activate twice: "));
		return;
	}
			
	bIsCapabilityActive = true;
	
#if !UE_BUILD_SHIPPING
	LogCapabilityDisplay(TEXT("Activating "));
#endif // !UE_BUILD_SHIPPING

	BP_OnActivate(ActivationParams);
}

EServerCapabilityDeactivation USecCapability::OnServer_ConfirmDeactivation(const FCapabilityDeactivationParams& DeactivationParams, FDeactivationSyncParams& SyncParams)
{
	if (!HasAuthority())
		return EServerCapabilityDeactivation::DontDeactivate;

	return BP_OnServer_ConfirmDeactivation(DeactivationParams, SyncParams);
}

EServerCapabilityDeactivation USecCapability::BP_OnServer_ConfirmDeactivation_Implementation(const FCapabilityDeactivationParams& DeactivationParams, FDeactivationSyncParams& SyncParams)
{
	return EServerCapabilityDeactivation::Deactivate;
}

void USecCapability::OnDeactivate(const FDeactivationSyncParams& SyncParams)
{
	FCapabilityDeactivationParams DeactivationParams = USecCapabilitySystemStatics::MakeDeactivationParamsFromSyncParams(SyncParams);
	if (!ensureMsgf(bIsCapabilityActive, TEXT("Attempting to deactivate %s, but this capability is not activated!"), *GetName()))
	{
		LogCapabilityWarning(TEXT("Not active, unable to deactivate: "));
		return;
	}

	bIsCapabilityActive = false;
	TimeActive = 0.0f;
#if !UE_BUILD_SHIPPING
	LogCapabilityDisplay(TEXT("Deactivating "));
#endif // !UE_BUILD_SHIPPING
		
	BP_OnDeactivate(DeactivationParams);
}

void USecCapability::TickActive(float DeltaTime)
{
	TimeActive += DeltaTime;
	TimeActiveTotal += DeltaTime;
	BP_TickActive(DeltaTime);
}

void USecCapability::PreTick(float DeltaTime)
{
	if (HasAuthority())
	{
		for (int32 Index = ReplicatedCrumbs.Items.Num() - 1; Index >= 0; --Index)
		{
			FSecReplicatedCapabilityCrumb& Crumb = ReplicatedCrumbs.Items[Index];
			//Crumb.TimeUntilRemoval -= DeltaTime;
		}
	}

	BP_PreTick(DeltaTime);
}

void USecCapability::ConsumeCrumbs(int32& OutSyncTag)
{
	for (int32 Index = CrumbList.Num() - 1; Index >= 0; --Index)
	{
		const FSecCapabilityCrumb Crumb = CrumbList[Index];

		if (Crumb.SyncParams.SyncTag == OutSyncTag)
		{
			if (Crumb.SyncType == ESecCapabilitySyncType::Activate)
			{
				OnActivate(Crumb.SyncParams);
			}
			else if (Crumb.SyncType == ESecCapabilitySyncType::Deactivate)
			{
				OnDeactivate(Crumb.SyncParams);
			}

			OutSyncTag++;
			CrumbList.RemoveAt(Index);
		}
	}
}

void USecCapability::BlockCapabilities(FName BlockingTag, UObject* Instigator)
{
	if (!ensure(CapabilitySystem != nullptr))
		return;

	CapabilitySystem->BlockCapabilities(BlockingTag, Instigator);

}

void USecCapability::UnblockCapabilities(FName BlockingTag, UObject* Instigator)
{
	if (!ensure(CapabilitySystem != nullptr))
		return;

	CapabilitySystem->UnblockCapabilities(BlockingTag, Instigator);
}

bool USecCapability::IsCapabilityActive() const
{
	return bIsCapabilityActive;
}

bool USecCapability::IsLocallyControlled() const
{
	return (CapabilitySystem ? CapabilitySystem->IsLocallyControlled() : false);
}

USecCapabilitySystemComponent* USecCapability::GetCapabilitySystemComponent() const
{
	return CapabilitySystem;
}

AActor* USecCapability::GetOwner() const
{
	return Owner;
}

ENetRole USecCapability::GetLocalRole() const
{
	return GetOwner()->GetLocalRole();
}

ENetRole USecCapability::GetRemoteRole() const
{
	return GetOwner()->GetRemoteRole();
}

bool USecCapability::HasAuthority() const
{
	return GetOwner()->HasAuthority();
}

USecAttributeComponent* USecCapability::GetAttribute() const
{
	return GetCapabilitySystemComponent()->AttributeComponent;
}

bool USecCapability::IsAction(FName ActionName) const
{
	if (!ensure(CapabilitySystem != nullptr))
		return false;
	return CapabilitySystem->IsAction(ActionName);
}

bool USecCapability::WasActionActivated(FName ActionName) const
{
	if (!ensure(CapabilitySystem != nullptr))
		return false;
	return CapabilitySystem->WasActionActivated(ActionName);
}

bool USecCapability::IsActionActivated(FName ActionName) const
{
	if (!ensure(CapabilitySystem != nullptr))
		return false;
	return CapabilitySystem->IsActionActivated(ActionName);
}

bool USecCapability::ConsumeAction(FName ActionName)
{
	if (!ensure(CapabilitySystem != nullptr))
		return false;
	return CapabilitySystem->ConsumeAction(ActionName);
}

void USecCapability::SetActionState(FName ActionName, ESecActionState ActionState)
{
	if (!ensure(CapabilitySystem != nullptr))
		return;
	CapabilitySystem->SetActionState(ActionName, ActionState);
}

void USecCapability::NetworkActivation(int32 InSyncTag)
{
	FActivationSyncParams SyncParams;
	PreActivation(SyncParams);
	OnActivate(SyncParams);
	SyncParams.SyncTag = InSyncTag;
	Server_Activate(SyncParams);
}

void USecCapability::NetworkDeactivation(int32 InSyncTag)
{
	FDeactivationSyncParams SyncParams;
	PreDeactivation(SyncParams);
	OnDeactivate(SyncParams);
	SyncParams.SyncTag = InSyncTag;
	Server_Deactivate(SyncParams);
}

void USecCapability::ServerOnlyActivation()
{
	FActivationSyncParams SyncParams;
	PreActivation(SyncParams);
	OnActivate(SyncParams);
	ServerOnly_Activate(SyncParams);
}

void USecCapability::RemoteActivation(int32 InRemoteSyncTag)
{
	LastRemoteSyncTag = InRemoteSyncTag;
	FSecCapabilityCrumb Crumb = CrumbList.Pop();
	OnActivate(Crumb.SyncParams);
}

void USecCapability::RemoteDeactivation(int32 InRemoteSyncTag)
{
	LastRemoteSyncTag = InRemoteSyncTag;
	FSecCapabilityCrumb Crumb = CrumbList.Pop();
	OnDeactivate(Crumb.SyncParams);
}

void USecCapability::Server_Activate_Implementation(FActivationSyncParams SyncParams)
{
	const FCapabilityActivationParams ActivationParams = USecCapabilitySystemStatics::MakeActivationParamsFromSyncParams(SyncParams);
	const EServerCapabilityActivation ServerActivationResults = OnServer_ConfirmActivation(ActivationParams, SyncParams);

	if (ServerActivationResults == EServerCapabilityActivation::Activate)
	{
		AddReplicatedCrumb(FSecReplicatedCapabilityCrumb(SyncParams, ESecCapabilitySyncType::Activate));
	}
}

void USecCapability::Server_Deactivate_Implementation(FDeactivationSyncParams SyncParams)
{
	const FCapabilityDeactivationParams DeactivationParams = USecCapabilitySystemStatics::MakeDeactivationParamsFromSyncParams(SyncParams);
	const EServerCapabilityDeactivation ServerDeactivationResults = OnServer_ConfirmDeactivation(DeactivationParams, SyncParams);

	if (ServerDeactivationResults == EServerCapabilityDeactivation::Deactivate)
	{
		AddReplicatedCrumb(FSecReplicatedCapabilityCrumb(SyncParams, ESecCapabilitySyncType::Deactivate));
	}
}

void USecCapability::ServerOnly_Activate_Implementation(FActivationSyncParams SyncParams)
{
	const FCapabilityActivationParams ActivationParams = USecCapabilitySystemStatics::MakeActivationParamsFromSyncParams(SyncParams);
	const EServerCapabilityActivation ServerActivationResults = OnServer_ConfirmActivation(ActivationParams, SyncParams);
}

void USecCapability::AddReplicatedCrumb(FSecReplicatedCapabilityCrumb InCrumb)
{
	InCrumb.TimeUntilRemoval = 5.5f;

	ReplicatedCrumbs.MarkItemDirty(InCrumb);

	bool bAdded = false;

	for (auto& Crumb : ReplicatedCrumbs.Items)
	{
		if (Crumb.TimeUntilRemoval < 0.0f)
		{
			Crumb = InCrumb;
			bAdded = true;
			break;
		}
	}

	if (!bAdded)
		ReplicatedCrumbs.Items.Add(InCrumb);

	OnRep_ReplicatedCrumbs();
}

FString USecCapability::GetCapabilityLogString(const FString& InLogString) const
{
	const FString LocalRoleString = GetLocalRoleAsString();
	const FString EndString = FString::Printf(TEXT("%s on %s - %s"), *GetName(), *Owner->GetName(), *LocalRoleString);
	const FString LogString = InLogString + EndString;
	return LogString;
}

void USecCapability::LogCapabilityDisplay(const FString& InLogString) const
{
	const FString LogString = GetCapabilityLogString(InLogString);
	UE_LOG(CapabilitySystemLog, Display, TEXT("%s"), *LogString);
}

void USecCapability::LogCapabilityWarning(const FString& InLogString) const
{
	const FString LogString = GetCapabilityLogString(InLogString);
	UE_LOG(CapabilitySystemLog, Warning, TEXT("%s"), *LogString);
}

FString USecCapability::GetLocalRoleAsString() const
{
	return USecUtilityStatics::EnumToString<ENetRole>(GetLocalRole());
}

int8 TestCompress(float Val, float Max)
{
	return 0;
}

uint8 TestCompress2(float Val, float Max)
{
	return FMath::RoundToInt(Val * 256.0f / Max) & 0xFF;
}

int32 USecCapability::TestFunction(float Val, float Max)
{
	uint8 Compressed = TestCompress2(Val, Max);
	float Uncompressed = (Compressed * Max / 256.0f);
	return static_cast<int32>(Compressed);
}

void USecCapability::OnRep_ReplicatedCrumbs()
{
	TArray<FSecCapabilityCrumb> NewCrumbs;
	
	for (const FSecReplicatedCapabilityCrumb& Crumb : ReplicatedCrumbs.Items)
	{
		// If any of the sync tags are larger than the last one, add them
		if (Crumb.SyncParams.SyncTag > LastRemoteSyncTag && !CrumbList.Contains(Crumb))
		{
			NewCrumbs.Add(Crumb);
		}
	}

	if (NewCrumbs.Num() == 0)
		return;

	CrumbList.Append(NewCrumbs);
	USecMathStatics::BubbleSortReversed(CrumbList);
}

ECapabilityActivation USecCapability::BP_ShouldActivate_Implementation() const
{
	return ECapabilityActivation::DontActivate;
}

ECapabilityDeactivation USecCapability::BP_ShouldDeactivate_Implementation() const
{
	return ECapabilityDeactivation::DontDeactivate;
}

void USecCapability::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USecCapability, ReplicatedCrumbs);
}
