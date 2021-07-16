#include "ObjectiveCallbackProxy.h"
#include "ObjectiveSystemActor.h"
#include "ObjectiveStatics.h"
#include "ObjectiveBase.h"
#include "ObjectiveSettings.h"
#include "Engine/World.h"
#include "Engine/Engine.h"

const FName UCreateObjectiveTask::HandleOnStartName(TEXT("HandleOnStart"));
const FName UCreateObjectiveTask::HandleOnCompleteName(TEXT("HandleOnComplete"));
const FName UCreateObjectiveTask::HandleOnFailName(TEXT("HandleOnFail"));

UCreateObjectiveTask* UCreateObjectiveTask::CreateObjective(const UObject* WorldContextObject, UObjectiveSettings* ObjectiveSettings)
{
	if (!HasAuthority(WorldContextObject))
	{
		UCreateObjectiveTask* CreateObjectiveTask = NewObject<UCreateObjectiveTask>();
		return CreateObjectiveTask;
	}

	// If the class type is empty...
	if (!ObjectiveSettings)
	{
		return nullptr;
	}
	
	if (AObjectiveSystemActor* ObjectiveSystemActor = UObjectiveStatics::GetObjectiveSystemActor(WorldContextObject))
	{
		 UObjectiveBase* NewObjective = ObjectiveSystemActor->CreateObjective(ObjectiveSettings);
		 UCreateObjectiveTask* CreateObjectiveTask = NewObject<UCreateObjectiveTask>();
		 CreateObjectiveTask->Objective = NewObjective;

		 CreateObjectiveTask->RegisterDelegates();

		 ObjectiveSystemActor->CreateObjectiveDelegate(ObjectiveSettings, EObjectiveCallbackType::EOCT_OnStart, CreateObjectiveTask->Internal_OnStart);
		 ObjectiveSystemActor->CreateObjectiveDelegate(ObjectiveSettings, EObjectiveCallbackType::EOCT_OnFail, CreateObjectiveTask->Internal_OnFail);
		 ObjectiveSystemActor->CreateObjectiveDelegate(ObjectiveSettings, EObjectiveCallbackType::EOCT_OnComplete, CreateObjectiveTask->Internal_OnComplete);

		 return CreateObjectiveTask;
	}

	return nullptr;
}

void UCreateObjectiveTask::Start(const UObject* WorldContextObject)
{
	if (!HasAuthority(WorldContextObject))
	{
		return;
	}

	Objective->Start();
}

void UCreateObjectiveTask::Complete(const UObject* WorldContextObject)
{
	if (!HasAuthority(WorldContextObject))
	{
		return;
	}

	Objective->Complete();
}

void UCreateObjectiveTask::Fail(const UObject* WorldContextObject)
{
	if (!HasAuthority(WorldContextObject))
	{
		return;
	}

	Objective->Fail();
}

void UCreateObjectiveTask::Progress(const UObject* WorldContextObject, int32 Value)
{
	if (!HasAuthority(WorldContextObject))
	{
		return;
	}

	Objective->Progress(Value);
}

void UCreateObjectiveTask::RegisterDelegates()
{
	Internal_OnStart.BindUFunction(this, HandleOnStartName);
	Internal_OnComplete.BindUFunction(this, HandleOnCompleteName);
	Internal_OnFail.BindUFunction(this, HandleOnFailName);
}

void UCreateObjectiveTask::HandleOnStart()
{
	OnStart.Broadcast();
}

void UCreateObjectiveTask::HandleOnComplete()
{
	OnComplete.Broadcast();
}

void UCreateObjectiveTask::HandleOnFail()
{
	OnFail.Broadcast();
}

bool UCreateObjectiveTask::HasAuthority(const UObject* WorldContextObject)
{
	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull))
	{
		return World->IsServer();
	}

	return false;
}
