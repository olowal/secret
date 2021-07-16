#pragma once

#include "CoreMinimal.h"
#include "SecTriggerCondition.generated.h"

class USecTriggerComponent;
class ASecCharacter;

DECLARE_DYNAMIC_DELEGATE_RetVal_TwoParams(bool, FSecTriggerConditionDelegate, USecTriggerComponent*, Trigger, ASecCharacter*, Character);
DECLARE_DELEGATE_RetVal_TwoParams(bool, FSecTriggerConditionCodeDelegate, USecTriggerComponent*, ASecCharacter*);

USTRUCT(BlueprintType)
struct FSecTriggerCondition
{
	GENERATED_BODY()

	FSecTriggerCondition() {}

	FSecTriggerCondition(FSecTriggerConditionDelegate InDelegate)
		: Delegate(InDelegate)
	{
	}

	FSecTriggerCondition(FSecTriggerConditionCodeDelegate InDelegate)
		: CodeDelegate(InDelegate)
	{
	}

	/* Condition to check. If it returns true, the trigger can be used. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FSecTriggerConditionDelegate Delegate;

	/* Whether the condition should not disable the visuals of the trigger. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bDisplayVisualsWhileDisabled = false;

	/* Whether the condition should only be checked locally, and always assume success on the interacting player's remote side. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bOnlyCheckOnPlayerControl = false;

	FSecTriggerConditionCodeDelegate CodeDelegate;
	int32 ConditionHandleId = -1;

	bool IsEnabled(USecTriggerComponent* Trigger, ASecCharacter* Player)
	{
		if (Delegate.IsBound())
			return Delegate.Execute(Trigger, Player);
		else if (CodeDelegate.IsBound())
			return CodeDelegate.Execute(Trigger, Player);
		else
			return false;
	}
};

USTRUCT(BlueprintType, Meta = (PassAsInt))
struct FSecTriggerConditionHandle
{
	GENERATED_BODY()

	FSecTriggerConditionHandle() {}
	FSecTriggerConditionHandle(int32 InHandleId)
		: HandleId(InHandleId)
	{
	}

	UPROPERTY()
	int32 HandleId = -1;
};
