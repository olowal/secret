#include "ObjectiveStatics.h"
#include "ObjectiveSystemActor.h"
#include "EngineUtils.h"
#include "Engine/Engine.h"
#include "Engine/World.h"

AObjectiveSystemActor* UObjectiveStatics::GetObjectiveSystemActor(const UObject* WorldContextObject)
{
	AObjectiveSystemActor* ObjectiveSystem = nullptr;

	if (GEngine)
	{
		if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull))
		{
			for (TActorIterator<AObjectiveSystemActor> It(World); It; ++It)
			{
				if (!ObjectiveSystem)
				{
					ObjectiveSystem = *It;
				}
				else
				{
					ensureMsgf(false, TEXT("Multiple instances of ObjectiveSystemActor detected"));
				}
			}
		}
	}

	ensureMsgf(ObjectiveSystem != nullptr, TEXT("Unable to locate an instance of ObjectiveSystemActor"));

	return ObjectiveSystem;
}

void UObjectiveStatics::StartObjective(const UObject* WorldContextObject, UObjectiveSettings* Objective)
{
	if (AObjectiveSystemActor* ObjectiveSystemActor = UObjectiveStatics::GetObjectiveSystemActor(WorldContextObject))
	{
		ObjectiveSystemActor->StartObjective(Objective);
	}
}

void UObjectiveStatics::CompleteObjective(const UObject* WorldContextObject, UObjectiveSettings* Objective)
{
	if (AObjectiveSystemActor* ObjectiveSystemActor = UObjectiveStatics::GetObjectiveSystemActor(WorldContextObject))
	{
		ObjectiveSystemActor->CompleteObjective(Objective);
	}
}

void UObjectiveStatics::FailObjective(const UObject* WorldContextObject, UObjectiveSettings* Objective)
{
	if (AObjectiveSystemActor* ObjectiveSystemActor = UObjectiveStatics::GetObjectiveSystemActor(WorldContextObject))
	{
		ObjectiveSystemActor->FailObjective(Objective);
	}
}

void UObjectiveStatics::ProgressObjective(const UObject* WorldContextObject, UObjectiveSettings* Objective, int32 Value)
{
	if (AObjectiveSystemActor* ObjectiveSystemActor = UObjectiveStatics::GetObjectiveSystemActor(WorldContextObject))
	{
		ObjectiveSystemActor->ProgressObjective(Objective, Value);
	}
}

void UObjectiveStatics::SetObjectiveUIHidden(const UObject* WorldContextObject, bool bHidden)
{
	if (AObjectiveSystemActor* ObjectiveSystemActor = UObjectiveStatics::GetObjectiveSystemActor(WorldContextObject))
	{
		ObjectiveSystemActor->SetUIHidden(bHidden);
	}
}

void UObjectiveStatics::CreateObjectiveDelegate(const UObject* WorldContextObject, UObjectiveSettings* Objective, EObjectiveCallbackType CallbackType, const FObjectiveCallbackSignature& Delegate)
{
	if (AObjectiveSystemActor* ObjectiveSystemActor = UObjectiveStatics::GetObjectiveSystemActor(WorldContextObject))
	{
		ObjectiveSystemActor->CreateObjectiveDelegate(Objective, CallbackType, Delegate);
	}
}
