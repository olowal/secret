#include "ObjectiveSystemActor.h"
#include "UnrealNetwork.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/GameStateBase.h"
#include "Engine/ActorChannel.h"
#include "EngineUtils.h"
#include "TimerManager.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ObjectiveBase.h"
#include "ObjectiveSettings.h"
#include "UI/ObjectiveHUDWidget.h"
#include "UI/ObjectiveWidget.h"

DEFINE_LOG_CATEGORY(ObjectiveSystemActorLog);

//const float AObjectiveSystemActor::DebugPrintDisplayTime = 10.0f;

AObjectiveSystemActor::AObjectiveSystemActor(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	bAlwaysRelevant = true;
	NetUpdateFrequency = 2.0f;
	NetPriority = 1.0f;

	DefaultHUDClass = UObjectiveHUDWidget::StaticClass();
	DefaultObjectiveWidgetClass = UObjectiveWidget::StaticClass();
	DefaultObjectiveClass = UObjectiveBase::StaticClass();
}

void AObjectiveSystemActor::BeginPlay()
{
	Super::BeginPlay();
	InitHUDWidget();
	SetUIHidden(false);
}

UObjectiveBase* AObjectiveSystemActor::CreateObjective(UObjectiveSettings* ObjectiveSettings)
{
	if (!HasAuthority())
	{
		return nullptr;
	}

	if (!ObjectiveSettings)
	{
		return nullptr;
	}

	UObjectiveBase* NewObjective = Internal_CreateObjective(ObjectiveSettings);

	OnRep_ObjectiveList();
	return NewObjective;
}

UObjectiveBase* AObjectiveSystemActor::Internal_CreateObjective(UObjectiveSettings* ObjectiveSettings, UObjectiveBase* ParentObjective)
{
	// Select the objective class from settings, if it has been changed, otherwise set to default.
	TSubclassOf<UObjectiveBase> ObjectiveClass = (*ObjectiveSettings->DefaultObjectiveClass) ? ObjectiveSettings->DefaultObjectiveClass : DefaultObjectiveClass;

	UObjectiveBase* NewObjective = NewObject<UObjectiveBase>(this, ObjectiveClass);
	NewObjective->Settings = ObjectiveSettings;
	ObjectiveList.Add(NewObjective);

	for (UObjectiveSettings* ChildObjectiveSetting : ObjectiveSettings->ChildrenObjectives)
	{
		if (UObjectiveBase* ChildObjective = Internal_CreateObjective(ChildObjectiveSetting))
		{
			ChildObjective->ParentObjective = NewObjective;
			NewObjective->Children.Add(ChildObjective);
		}
	}

	return NewObjective;
}

void AObjectiveSystemActor::InitHUDWidget()
{
	if (!HUDInstance)
	{
		HUDInstance = CreateWidget<UObjectiveHUDWidget>(GetWorld(), DefaultHUDClass);
	}
}

void AObjectiveSystemActor::CreateObjectiveDelegate(UObjectiveSettings* Objective, EObjectiveCallbackType CallbackType, const FObjectiveCallbackSignature& Delegate)
{
	if (CallbackType == EObjectiveCallbackType::EOCT_OnStart)
	{
		Internal_CreateDelegate(Objective, Delegate, OnStartDelegateContainers);
	}
	else if (CallbackType == EObjectiveCallbackType::EOCT_OnComplete)
	{
		Internal_CreateDelegate(Objective, Delegate, OnCompleteDelegateContainers);
	}
	else if (CallbackType == EObjectiveCallbackType::EOCT_OnFail)
	{
		Internal_CreateDelegate(Objective, Delegate, OnFailDelegateContainers);
	}
}

void AObjectiveSystemActor::BroadcastDelegates(UObjectiveSettings* Objective, EObjectiveCallbackType CallbackType)
{
	if (CallbackType == EObjectiveCallbackType::EOCT_OnStart)
	{
		Internal_BroadcastDelegates(Objective, OnStartDelegateContainers);
	}
	else if (CallbackType == EObjectiveCallbackType::EOCT_OnComplete)
	{
		Internal_BroadcastDelegates(Objective, OnCompleteDelegateContainers);
	}
	else if (CallbackType == EObjectiveCallbackType::EOCT_OnFail)
	{
		Internal_BroadcastDelegates(Objective, OnFailDelegateContainers);
	}
}

void AObjectiveSystemActor::StartObjective(UObjectiveSettings* ObjectiveSettings)
{
	if (!HasAuthority())
	{
		return;
	}

	if (UObjectiveBase* Objective = FindObjectiveFromSettings(ObjectiveSettings))
	{
		Objective->Start();
	}
	else
	{

	}
}

void AObjectiveSystemActor::CompleteObjective(UObjectiveSettings* ObjectiveSettings)
{
	if (!HasAuthority())
	{
		return;
	}

	if (UObjectiveBase* Objective = FindObjectiveFromSettings(ObjectiveSettings))
	{
		Objective->Complete();
	}
}

void AObjectiveSystemActor::FailObjective(UObjectiveSettings* ObjectiveSettings)
{
	if (!HasAuthority())
	{
		return;
	}

	if (UObjectiveBase* Objective = FindObjectiveFromSettings(ObjectiveSettings))
	{
		Objective->Fail();
	}
}

void AObjectiveSystemActor::ProgressObjective(UObjectiveSettings* ObjectiveSettings, int32 Value)
{
	if (!HasAuthority())
	{
		return;
	}

	if (UObjectiveBase* Objective = FindObjectiveFromSettings(ObjectiveSettings))
	{
		Objective->Progress(Value);
	}
}

void AObjectiveSystemActor::AddToUI(UObjectiveBase* Objective)
{
	HUDInstance->OnObjectiveStart(Objective);
}

void AObjectiveSystemActor::SetUIHidden(bool bUIHidden)
{
	bUIHidden ? HUDInstance->RemoveFromParent() : HUDInstance->AddToViewport();
}

bool AObjectiveSystemActor::ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
	WroteSomething |= Channel->ReplicateSubobjectList(ObjectiveList, *Bunch, *RepFlags);
	return WroteSomething;
}

void AObjectiveSystemActor::OnRep_ObjectiveList()
{
	InitHUDWidget();
	
	for (UObjectiveBase* Objective : ObjectiveList)
	{
		if (Objective)
		{
			Objective->ObjectiveSystem = this;
			Objective->InitObjective();
		}
	}
}

UObjectiveBase* AObjectiveSystemActor::FindObjectiveFromSettings(UObjectiveSettings* ObjectiveSettings)
{
	for (UObjectiveBase* Objective : ObjectiveList)
	{
		if (Objective->Settings == ObjectiveSettings)
		{
			return Objective;
		}
	}

	return nullptr;
}

void AObjectiveSystemActor::Multicast_AddObjective_Implementation(UObjectiveBase* Objective)
{

}

void AObjectiveSystemActor::Internal_BroadcastDelegates(UObjectiveSettings* Objective, TArray<FObjectiveDelegateContainer>& DelegatesToBroadcast)
{
	int32 Index = DelegatesToBroadcast.Find(Objective);

	if (Index != INDEX_NONE)
	{
		FObjectiveDelegateInfo DelegateInfo;
		DelegateInfo.Objective = Objective;

		for (FObjectiveCallbackSignature& Delegate : DelegatesToBroadcast[Index].Delegates)
		{
			Delegate.Execute(DelegateInfo);
		}
	}
}

void AObjectiveSystemActor::Internal_CreateDelegate(UObjectiveSettings* Objective, const FObjectiveCallbackSignature& Delegate, TArray<FObjectiveDelegateContainer>& DelegateList)
{
	int32 Index = DelegateList.Find(Objective);

	if (Index != INDEX_NONE)
	{
		DelegateList[Index].Delegates.Add(Delegate);
	}
	else
	{
		FObjectiveDelegateContainer DelegateContainer;
		DelegateContainer.Objective = Objective;
		DelegateContainer.Delegates.Add(Delegate);
		DelegateList.Add(DelegateContainer);
	}
}

void AObjectiveSystemActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AObjectiveSystemActor, ObjectiveList);
}

