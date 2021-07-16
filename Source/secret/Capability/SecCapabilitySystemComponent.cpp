#include "SecCapabilitySystemComponent.h"
#include "UnrealNetwork.h"
#include "Engine/ActorChannel.h"
#include "GameFramework/Pawn.h"
#include "SecCapability.h"
#include "SecCapabilitySheet.h"
#include "Attribute/SecAttributeComponent.h"
#include "AIController.h"

USecCapabilitySystemComponent::USecCapabilitySystemComponent(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);

	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void USecCapabilitySystemComponent::BeginPlay()
{	
	Super::BeginPlay();
}

void USecCapabilitySystemComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	for (auto& It : ActionAttributes)
	{
		FSecActionState& ActionState = It.Value;
		if (ActionState.Current == ESecActionState::ActiveForOneFrame)
			ActionState.Current = ESecActionState::Inactive;

		ActionState.Previous = ActionState.Current;
	}

	const bool bIsLocallyControlled = IsLocallyControlled();

	for (USecCapability* CurrentCapability : CapabilityCollection)
	{
#if !UE_BUILD_SHIPPING
		if (!ensure(CurrentCapability != nullptr))
			continue;
#endif // !UE_BUILD_SHIPPING

		CurrentCapability->PreTick(DeltaTime);

		if (CurrentCapability->ShouldCheckActivation())
		{
			const ECapabilityActivation ActivationInfo = CurrentCapability->ShouldActivate(BlockingTags, RemoteSyncTag);
			if (ActivationInfo != ECapabilityActivation::DontActivate)
			{
				if (ActivationInfo == ECapabilityActivation::Network && bIsLocallyControlled)
				{
					CurrentCapability->NetworkActivation(++LocalSyncTag);
				}
				else if (ActivationInfo == ECapabilityActivation::Server && bIsLocallyControlled)
				{
					CurrentCapability->ServerOnlyActivation();
				}
				else if (ActivationInfo == ECapabilityActivation::RemoteActivate && !bIsLocallyControlled)
				{
					CurrentCapability->RemoteActivation(RemoteSyncTag);
					RemoteSyncTag++;
				}
				else if (ActivationInfo == ECapabilityActivation::Local)
				{
					CurrentCapability->OnActivate(FActivationSyncParams());
				}
			}
		}
		else if (CurrentCapability->ShouldCheckDeactivation())
		{
			const ECapabilityDeactivation DeactivationInfo = CurrentCapability->ShouldDeactivate(BlockingTags, RemoteSyncTag);
			if (DeactivationInfo != ECapabilityDeactivation::DontDeactivate)
			{
				if (DeactivationInfo == ECapabilityDeactivation::Network && bIsLocallyControlled)
				{
					CurrentCapability->NetworkDeactivation(++LocalSyncTag);
				}
				else if (DeactivationInfo == ECapabilityDeactivation::RemoteDeactivate && !bIsLocallyControlled)
				{
					CurrentCapability->RemoteDeactivation(RemoteSyncTag);
					RemoteSyncTag++;
				}
				else if (DeactivationInfo == ECapabilityDeactivation::Local)
				{
					CurrentCapability->OnDeactivate(FDeactivationSyncParams());
				}
			}
		}

		if(CurrentCapability->ShouldTickActive())
			CurrentCapability->TickActive(DeltaTime);
	}
}

bool USecCapabilitySystemComponent::ReplicateSubobjects(UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
	for (USecCapability* Capability : CapabilityCollection)
	{
		if(Capability->IsSupportedForNetworking())
			WroteSomething |= Channel->ReplicateSubobject(Capability, *Bunch, *RepFlags);
	}

	return WroteSomething;
}

void USecCapabilitySystemComponent::AddCapability(TSubclassOf<USecCapability> CapabilityClass)
{
	if (!HasAuthority())
		return;

	if (!ensure(CapabilityClass != nullptr))
		return;

	if (DoesCapabilityExist(CapabilityClass))
		return;

	USecCapability* NewCapability = NewObject<USecCapability>(GetOwner(), CapabilityClass);
	check(NewCapability != nullptr);
	ReplicatedCapabilities.Add(NewCapability);
	OnRep_ReplicatedCapabilities();
}

void USecCapabilitySystemComponent::ApplySheet(USecCapabilitySheet* Sheet, ESecScriptPriority Priority)
{
	if (!HasAuthority())
		return;

	if (!ensure(Sheet != nullptr))
		return;

	// Let's check that we don't already have this sheet.
	for (const auto& SheetEntry : CapabilitySheetSyncContainer.Items)
	{
		if (SheetEntry.CapabilitySheet == Sheet)
		{
			if (!ensureMsgf(SheetEntry.CapabilitySheet != Sheet, TEXT("This CapabilitySheet is already applied: %s. Unable to apply the same sheet multiple times."), *Sheet->GetName()))
				return;
		}
	}

	if (Sheet->AIController != nullptr)
	{
		APawn* Pawn = Cast<APawn>(GetOwner());

		if (ensureMsgf(Pawn != nullptr, TEXT("Sheet: %s attempted to create and possess %s. But %s is not of type Pawn."), *Sheet->GetName(), *GetOwner()->GetName(), *GetOwner()->GetName()))
		{
			// Following is copy-pasted and slightly modified from SpawnDefaultController() in Pawn.cpp.
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.Instigator = Pawn->GetInstigator();
			SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			SpawnInfo.OverrideLevel = Pawn->GetLevel();
			SpawnInfo.ObjectFlags |= RF_Transient;	// We never want to save AI controllers into a map
			AController* NewController = GetWorld()->SpawnActor<AController>(Sheet->AIController, GetOwner()->GetActorLocation(), GetOwner()->GetActorRotation(), SpawnInfo);
			if (NewController != nullptr)
			{
				// if successful will result in setting this->Controller 
				// as part of possession mechanics
				NewController->Possess(Pawn);
			}
		}
	}

	FSecCapabilitySheetSyncEntry SheetSyncEntry;
	SheetSyncEntry.CapabilitySheet = Sheet;
	SheetSyncEntry.Priority = Priority;

	for (auto CapabilityClass : Sheet->Capabilities)
	{
		if (DoesCapabilityExist(CapabilityClass))
			continue;

		USecCapability* NewCapability = NewObject<USecCapability>(GetOwner(), CapabilityClass);
		check(NewCapability != nullptr);
		SheetSyncEntry.CapabilityCollection.Add(NewCapability);
	}

	CapabilitySheetSyncContainer.MarkItemDirty(SheetSyncEntry);
	CapabilitySheetSyncContainer.Items.Add(SheetSyncEntry);
	OnRep_CapabilitySheetContainer();
}

void USecCapabilitySystemComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USecCapabilitySystemComponent, CapabilitySheetSyncContainer);
}

bool USecCapabilitySystemComponent::HasAuthority() const
{
	return GetOwner() && GetOwner()->HasAuthority();
}

bool USecCapabilitySystemComponent::IsLocallyControlled() const
{
	if (const APawn* OwnerPawn = Cast<APawn>(GetOwner()))
		return OwnerPawn->IsLocallyControlled();

	return GetOwner()->HasAuthority();
}

void USecCapabilitySystemComponent::BlockCapabilities(FName BlockingTag, UObject* Instigator)
{
	TArray<UObject*>* Blockers = BlockingInstigators.Find(BlockingTag);
	if (Blockers != nullptr && !(*Blockers).Contains(Instigator))
	{
		(*Blockers).Add(Instigator);
	}
	else
	{
		TArray<UObject*> NewBlockersList;
		NewBlockersList.Add(Instigator);

		BlockingInstigators.Add(BlockingTag, NewBlockersList);
		if (!BlockingTags.Contains(BlockingTag))
		{
			BlockingTags.Add(BlockingTag);
		}
	}
}

void USecCapabilitySystemComponent::UnblockCapabilities(FName BlockingTag, UObject* Instigator)
{
	if (TArray<UObject*> * Blockers = BlockingInstigators.Find(BlockingTag))
	{
		(*Blockers).Remove(Instigator);

if ((*Blockers).Num() == 0)
if ((*Blockers).Num() == 0)
{
	BlockingInstigators.Remove(BlockingTag);
	BlockingTags.Remove(BlockingTag);
}
	}
}

bool USecCapabilitySystemComponent::IsAction(FName ActionName) const
{
	const FSecActionState* ActionState = ActionAttributes.Find(ActionName);
	if (!ActionState)
		return false;
	const ESecActionState CurrentActionState = ActionState->Current;
	return CurrentActionState != ESecActionState::Inactive;
}

bool USecCapabilitySystemComponent::WasActionActivated(FName ActionName) const
{
	const FSecActionState* ActionState = ActionAttributes.Find(ActionName);
	if (!ActionState)
		return false;
	const bool bIsActionActive = ActionState->Current != ESecActionState::Inactive;
	const bool bWasActionActive = ActionState->Previous != ESecActionState::Inactive;
	return bWasActionActive && !bIsActionActive;
}

bool USecCapabilitySystemComponent::IsActionActivated(FName ActionName) const
{
	const FSecActionState* ActionState = ActionAttributes.Find(ActionName);
	if (!ActionState)
		return false;
	const bool bIsActionActive = ActionState->Current != ESecActionState::Inactive;
	const bool bWasActionActive = ActionState->Previous != ESecActionState::Inactive;
	return !bWasActionActive && bIsActionActive;
}

bool USecCapabilitySystemComponent::ConsumeAction(FName ActionName)
{
	FSecActionState* ActionState = ActionAttributes.Find(ActionName);
	if (!ActionState)
		return false;

	if (ActionState->Current == ESecActionState::Inactive)
		return false;

	ActionState->Current = ESecActionState::Inactive;
	return true;
}

void USecCapabilitySystemComponent::SetActionState(FName ActionName, ESecActionState ActionState)
{
	ActionAttributes.FindOrAdd(ActionName).Current = ActionState;
}

bool USecCapabilitySystemComponent::DoesCapabilityExist(TSubclassOf<USecCapability> CapabilityClass) const
{
	for (USecCapability* CapabilityToCompare : CapabilityCollection)
	{
		TSubclassOf<USecCapability> CapabilityClassToTest = CapabilityToCompare->GetClass();

		if (!ensureMsgf(CapabilityClassToTest.GetDefaultObject() != CapabilityClass.GetDefaultObject(), TEXT("%s Already exists."), *CapabilityClass.GetDefaultObject()->GetName()))
			return true;
	}

	return false;
}

void USecCapabilitySystemComponent::OnRegister()
{
	Super::OnRegister();

	AttributeComponent = GetOwner()->FindComponentByClass<USecAttributeComponent>();
	ensureMsgf(AttributeComponent != nullptr, TEXT("Unable to locate AttributeComponent in %s. This must be a part of the CapabilitySystem."), *GetOwner()->GetName());
}

void USecCapabilitySystemComponent::OnRep_CapabilitySheetContainer()
{
	if (!ensure(AttributeComponent != nullptr))
		return;
	
	for (auto& SheetEntry : CapabilitySheetSyncContainer.Items)
	{
		const USecCapabilitySheet* CapabilitySheet = SheetEntry.CapabilitySheet;

		if (!ensure(CapabilitySheet != nullptr))
			continue;

		const ESecCapabilitySheetSyncState SyncState = SheetEntry.SyncState;
		
		if (SyncState == ESecCapabilitySheetSyncState::Apply && !SheetEntry.bClientApplied)
		{
			if (SheetEntry.CapabilitySheet->AttributeSheet != nullptr)
			{
				AttributeComponent->ApplyAttributeSheet(SheetEntry.CapabilitySheet->AttributeSheet, this);
			}

			for (TSubclassOf<UActorComponent> ComponentClass : CapabilitySheet->Components)
			{
				if (!ensureMsgf(ComponentClass.GetDefaultObject() != nullptr, TEXT("ComponentClass entry is not valid.")))
					continue;

				UActorComponent* NewComponent = NewObject<UActorComponent>(GetOwner(), ComponentClass);
				check(NewComponent != nullptr);
				NewComponent->SetNetAddressable();
				NewComponent->RegisterComponent();
				SheetEntry.ComponentInstances.Add(NewComponent);
			}

			for (USecCapability* Capability : SheetEntry.CapabilityCollection)
			{
				Capability->Owner = GetOwner();
				Capability->CapabilitySystem = this;
				Capability->Setup();

				const int32 TickOrder = Capability->TickOrder;
				bool bWasAdded = false;

				for (int32 Index = 0, Num = CapabilityCollection.Num(); Index < Num; ++Index)
				{
					if (CapabilityCollection[Index]->TickOrder > TickOrder)
					{
						CapabilityCollection.Insert(Capability, Index);
						bWasAdded = true;
						break;
					}
				}

				if (!bWasAdded)
					CapabilityCollection.Add(Capability);
			}
			
			SheetEntry.bClientApplied = true;
		}
		else if (SyncState == ESecCapabilitySheetSyncState::Remove && SheetEntry.bClientApplied)
		{
			// TODO: Implement
			// We don't have a need for removing sheets at this moment.
		}
	}
}

void USecCapabilitySystemComponent::OnRep_ReplicatedCapabilities()
{
	for (USecCapability* Capability : ReplicatedCapabilities)
	{
		if (!Capability->HasRunSetup())
		{
			Capability->Owner = GetOwner();
			Capability->CapabilitySystem = this;
			Capability->Setup();

			const int32 TickOrder = Capability->TickOrder;
			bool bWasAdded = false;

			for (int32 Index = 0, Num = CapabilityCollection.Num(); Index < Num; ++Index)
			{
				if (CapabilityCollection[Index]->TickOrder > TickOrder)
				{
					CapabilityCollection.Insert(Capability, Index);
					bWasAdded = true;
					break;
				}
			}

			if (!bWasAdded)
				CapabilityCollection.Add(Capability);
		}
	}
}
