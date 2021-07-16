#pragma once

#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "SecBTTask_MoveTo.generated.h"

UCLASS()
class SECRET_API USecBTTask_MoveTo : public UBTTask_MoveTo
{
	GENERATED_BODY()
public:

protected:
	virtual UAITask_MoveTo* PrepareMoveTask(UBehaviorTreeComponent& OwnerComp, UAITask_MoveTo* ExistingTask, FAIMoveRequest& MoveRequest) override;
};
