#include "SecNetObject.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "UnrealNetwork.h"

int32 USecNetObject::GetFunctionCallspace(UFunction* Function, FFrame* Stack)
{
	AActor* OwnerActor = CastChecked<AActor>(GetOuter(), ECastCheckedType::NullAllowed);
	return (OwnerActor ? OwnerActor->GetFunctionCallspace(Function, Stack) : FunctionCallspace::Local);
}

bool USecNetObject::CallRemoteFunction(UFunction* Function, void* Parameters, FOutParmRec* OutParms, FFrame* Stack)
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

bool USecNetObject::IsSupportedForNetworking() const
{
	return true;
}

void USecNetObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	UBlueprintGeneratedClass* BPClass = Cast<UBlueprintGeneratedClass>(GetClass());
	if (BPClass != NULL)
		BPClass->GetLifetimeBlueprintReplicationList(OutLifetimeProps);

	// Angelscript
	if (GetClass()->ScriptTypePtr != nullptr)
		GetClass()->GetLifetimeScriptReplicationList(OutLifetimeProps);
}
