#include "SecBTTask_MoveTo.h"
#include "AI/Controller/SecAIController.h"

UAITask_MoveTo* USecBTTask_MoveTo::PrepareMoveTask(UBehaviorTreeComponent& OwnerComp, UAITask_MoveTo* ExistingTask, FAIMoveRequest& MoveRequest)
{
	if (!MoveRequest.IsMoveToActorRequest())
	{
		ASecAIController* Controller = Cast<ASecAIController>(OwnerComp.GetOwner());

		if (Controller)
		{
			FVector TargetLocation = MoveRequest.GetGoalLocation();
			if (Controller->ValidateMoveToLocation(TargetLocation))
			{
				MoveRequest.UpdateGoalLocation(TargetLocation);
			}
		}
	}


	return Super::PrepareMoveTask(OwnerComp, ExistingTask, MoveRequest);
}
