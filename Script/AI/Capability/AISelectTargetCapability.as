import AI.Perception.AITargetingComponent;
import AI.Perception.SecAIPerceptionComponent;

class UAISelectTargetCapability : USecCapability
{
	UAITargetingComponent TargetingComp;
	USecAIPerceptionComponent PerceptionComp;

	UFUNCTION(BlueprintOverride)
	void Setup()
	{
		TargetingComp = UAITargetingComponent::Get(Owner);
		PerceptionComp = USecAIPerceptionComponent::Get(Owner);
	}

	UFUNCTION(BlueprintOverride)
	ECapabilityActivation ShouldActivate() const
	{
		return ECapabilityActivation::Network;
	}

	UFUNCTION(BlueprintOverride)
	void TickActive(float DeltaTime)
	{
		if(TargetingComp.CurrentTarget == nullptr && PerceptionComp.NumTargets() > 0)
		{
			TargetingComp.CurrentTarget = PerceptionComp.GetTargetWithHighestAggro();
		}
	}

    UFUNCTION(BlueprintOverride)
    ECapabilityDeactivation ShouldDeactivate() const
    {
		return ECapabilityDeactivation::DontDeactivate;
    }
}
