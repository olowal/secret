
class UBTTask_Block : UBTTask_BlueprintBase
{
	UFUNCTION(BlueprintOverride)
	void ExecuteAI(AAIController AIController, APawn ControlledPawn)
	{
		ControlledPawn.SetActionState(GameplayAction::Block, ESecActionState::ActiveForOneFrame);
		FinishExecute(true);
	}
}

class UBTTask_StopBlock : UBTTask_BlueprintBase
{
	UFUNCTION(BlueprintOverride)
	void ExecuteAI(AAIController AIController, APawn ControlledPawn)
	{
		ControlledPawn.SetActionState(GameplayAction::Block, ESecActionState::ActiveForOneFrame);
		FinishExecute(true);
	}
}
