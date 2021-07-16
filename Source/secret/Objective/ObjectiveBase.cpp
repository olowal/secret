#include "ObjectiveBase.h"
#include "ObjectiveSystemActor.h"
#include "UI/ObjectiveWidget.h"
#include "ObjectiveSettings.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Engine/Engine.h"
#include "UnrealNetwork.h"

void UObjectiveBase::InitObjective()
{
	if (bInitialized)
	{
		return;
	}

	bInitialized = true;
	// Done for everyone, the rest is only for authority.

	if (ObjectiveState != EObjectiveState::EOS_None)
	{
		SetupUI();
	}

	if (!HasAuthority())
	{
		return;
	}

	ReplicatedCounter.Target = Settings->Counter.Target;
	ReplicatedCounter.Current = Settings->Counter.Start;

	ReplicatedTimer.Target = Settings->Timer.Target;
	ReplicatedTimer.Current = Settings->Timer.Start;

	if (Settings->bUseTimer)
	{
		GetWorld()->GetTimerManager().SetTimer(TimerReplicationHandle, this, &UObjectiveBase::ReplicateTimers, 1.0f, true);
	}
}

void UObjectiveBase::OnDestroyObjective()
{
	if (TimerReplicationHandle.IsValid())
	{
		if (const UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(TimerReplicationHandle);
		}
	}
}

void UObjectiveBase::Tick(float DeltaTime)
{
	if (ReplicatedTimer.bPaused || ReplicatedTimer.bDone)
	{
		return;
	}

	ReplicatedTimer.Current = FMath::FInterpConstantTo(ReplicatedTimer.Current, ReplicatedTimer.Target, DeltaTime, 1.0f);
	if(WidgetInstance)
		WidgetInstance->OnTimerTick(CreateUIInfo());

	if (HasAuthority() && FMath::IsNearlyEqual(ReplicatedTimer.Current, ReplicatedTimer.Target))
	{
		ReplicatedTimer.bDone = true;
	}
}

bool UObjectiveBase::IsTickable() const
{
	return Settings && Settings->bUseTimer && !ReplicatedTimer.bPaused && !ReplicatedTimer.bDone;
}

int32 UObjectiveBase::GetFunctionCallspace(UFunction* Function, FFrame* Stack)
{
	AActor* OwnerActor = CastChecked<AActor>(GetOuter(), ECastCheckedType::NullAllowed);
	return (OwnerActor ? OwnerActor->GetFunctionCallspace(Function, Stack) : FunctionCallspace::Local);
}

bool UObjectiveBase::CallRemoteFunction(UFunction* Function, void* Parameters, FOutParmRec* OutParms, FFrame* Stack)
{
	check(!HasAnyFlags(RF_ClassDefaultObject));
	check(GetOuter() != nullptr);

	AActor* OwnerActor = CastChecked<AActor>(GetOuter());

	bool bProcessed = false;

	FWorldContext* const Context = GEngine->GetWorldContextFromWorld(GetWorld());
	if (Context != nullptr)
	{
		for (FNamedNetDriver& Driver : Context->ActiveNetDrivers)
		{
			if (Driver.NetDriver != nullptr && Driver.NetDriver->ShouldReplicateFunction(OwnerActor, Function))
			{
				Driver.NetDriver->ProcessRemoteFunction(OwnerActor, Function, Parameters, OutParms, Stack, this);
				bProcessed = true;
			}
		}
	}

	return bProcessed;
}

TStatId UObjectiveBase::GetStatId() const
{
	return UObject::GetStatID();
}

UWorld* UObjectiveBase::GetWorld() const
{
	return ObjectiveSystem ? ObjectiveSystem->GetWorld() : nullptr;
}

void UObjectiveBase::Start()
{
	if (!HasAuthority())
	{
		return;
	}

	if (ObjectiveState == EObjectiveState::EOS_None)
	{
		ObjectiveState = EObjectiveState::EOS_InProgress;
		OnRep_ObjectiveState(EObjectiveState::EOS_None);
	}

	for (UObjectiveBase* Child : Children)
	{
		Child->Start();
	}
}

void UObjectiveBase::Complete()
{
	if (!HasAuthority())
	{
		return;
	}

	if (ObjectiveState == EObjectiveState::EOS_InProgress)
	{
		ObjectiveState = EObjectiveState::EOS_Completed;
		OnRep_ObjectiveState(EObjectiveState::EOS_InProgress);
	}
}

void UObjectiveBase::Fail()
{
	if (!HasAuthority())
	{
		return;
	}
	
	if (ObjectiveState == EObjectiveState::EOS_InProgress)
	{
		ObjectiveState = EObjectiveState::EOS_Failed;
		OnRep_ObjectiveState(EObjectiveState::EOS_InProgress);
	}
}

void UObjectiveBase::Progress(int32 Value)
{
	if (!HasAuthority())
	{
		return;
	}

	int32 Direction = FMath::Sign(ReplicatedCounter.Target - Settings->Counter.Start);
	ReplicatedCounter.Current = Direction > 0 ? FMath::Min(ReplicatedCounter.Current + Value, ReplicatedCounter.Target) : FMath::Max(ReplicatedCounter.Current + Value, ReplicatedCounter.Target);
	OnRep_Counter();
}

bool UObjectiveBase::HasAuthority() const
{
	return ObjectiveSystem && ObjectiveSystem->HasAuthority();
}

UObjectiveWidget* UObjectiveBase::SetupWidget()
{
	if (WidgetInstance)
	{
		WidgetInstance->RemoveFromParent();
	}
	
	TSubclassOf<UObjectiveWidget> WidgetClass = (*Settings->OverrideDefaultWidgetClass) ? Settings->OverrideDefaultWidgetClass : ObjectiveSystem->DefaultObjectiveWidgetClass;
	WidgetInstance = CreateWidget<UObjectiveWidget>(GetWorld(), WidgetClass);
	return WidgetInstance;
}

void UObjectiveBase::SetupUI()
{
	ObjectiveSystem->AddToUI(this);
	WidgetInstance->SetupObjective(CreateUIInfo());
}

FObjectiveUIInfo UObjectiveBase::CreateUIInfo() const
{
	FObjectiveUIInfo UIInfo;
	UIInfo.Settings = Settings;
	UIInfo.Description = Settings->ObjectiveDescription;
	UIInfo.bUseCounter = Settings->bUseCounter;
	UIInfo.Counter = FObjectiveCounterUIInfo(Settings->Counter.Start, ReplicatedCounter.Target, ReplicatedCounter.Current);
	UIInfo.bUseTimer = Settings->bUseTimer;
	UIInfo.Timer = FObjectiveTimerUIInfo(Settings->Timer.Start, ReplicatedTimer.Target, ReplicatedTimer.Current);
	return UIInfo;
}

void UObjectiveBase::ReplicateTimers()
{
	Multicast_UpdateTimers(ReplicatedTimer.Current);
}

void UObjectiveBase::OnRep_Counter()
{
	if (!bInitialized)
	{
		return;
	}

	if (WidgetInstance && Settings)
	{
		WidgetInstance->OnProgress(CreateUIInfo());
	}
}

void UObjectiveBase::OnRep_ObjectiveState(EObjectiveState OldObjectiveState)
{
	if (!bInitialized)
	{
		return;
	}

	if (OldObjectiveState == EObjectiveState::EOS_None && ObjectiveState == EObjectiveState::EOS_InProgress)
	{
		SetupUI();
		ObjectiveSystem->BroadcastDelegates(Settings, EObjectiveCallbackType::EOCT_OnStart);
		
	}
	else if (ObjectiveState == EObjectiveState::EOS_Completed)
	{
		WidgetInstance->OnObjectiveComplete(CreateUIInfo());
		ObjectiveSystem->BroadcastDelegates(Settings, EObjectiveCallbackType::EOCT_OnComplete);
	}
	else if (ObjectiveState == EObjectiveState::EOS_Failed)
	{
		WidgetInstance->OnObjectiveFail(CreateUIInfo());
		ObjectiveSystem->BroadcastDelegates(Settings, EObjectiveCallbackType::EOCT_OnFail);
	}
}

void UObjectiveBase::Multicast_UpdateTimers_Implementation(float NewTime)
{
	// No point in attempting to sync timers on authority.
	if (HasAuthority())
	{
		return;
	}

	ReplicatedTimer.Current = NewTime;
}

void UObjectiveBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UObjectiveBase, ParentObjective);
	DOREPLIFETIME(UObjectiveBase, Settings);
	DOREPLIFETIME(UObjectiveBase, ObjectiveState);
	DOREPLIFETIME(UObjectiveBase, ReplicatedTimer);
	DOREPLIFETIME(UObjectiveBase, ReplicatedCounter);
}
