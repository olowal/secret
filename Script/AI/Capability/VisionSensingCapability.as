import AI.Perception.SecAIPerceptionComponent;
import AI.Perception.SecSensingComponent;

import void GetSensingComponents(TArray<USecSensingComponent>& OutSensingComponents) from "GameInstance.ScriptSecGameInstance";
import AI.SecAIStateComponent;

class UVisionSensingCapability_OLD : USecCapability
{
	USecAIPerceptionComponent PerceptionComp;
	USecAIStateComponent AIStateComp;
	TArray<USecSensingComponent> SensingCollection;	// Gathered from global available sensing components.

	TArray<USecSensingComponent> VisibleTargets;

	UFUNCTION(BlueprintOverride)
	void Setup()
	{
		PerceptionComp = USecAIPerceptionComponent::Get(Owner);
		AIStateComp = USecAIStateComponent::Get(Owner);
	}

	UFUNCTION(BlueprintOverride)
	ECapabilityActivation ShouldActivate() const
	{
		return ECapabilityActivation::Network;
	}

	UFUNCTION(BlueprintOverride)
	void TickActive(float DeltaTime)
	{
		GetSensingComponents(SensingCollection);

		for(int Index = VisibleTargets.Num() - 1; Index >= 0; --Index)
		{
			USecSensingComponent SensingComp = VisibleTargets[Index];
			
			if(!IsTargetVisible(SensingComp))
			{
				VisibleTargets.RemoveAt(Index);
				PerceptionComp.Handle_OnTargetLost(SensingComp);
			}
		}

		for(USecSensingComponent SensingComp : SensingCollection)
		{
			if(!VisibleTargets.Contains(SensingComp))
			{
				if(IsTargetVisible(SensingComp))
				{
					VisibleTargets.AddUnique(SensingComp);
					PerceptionComp.Handle_OnTargetSeen(SensingComp);
				}
			}
		}
	}

	bool IsTargetVisible(USecSensingComponent SensingComp) const
	{
		if(PerceptionComp.IsPointWithinPeripheralVision(SensingComp.VisionCenterLocation)
		&& PerceptionComp.HasLineOfSightToActor(SensingComp.Owner, SensingComp.VisionCenterLocation))
		{
			return true;
		}

		return false;
	}

	UFUNCTION(BlueprintOverride)
	ECapabilityDeactivation ShouldDeactivate() const
	{
		return ECapabilityDeactivation::DontDeactivate;
	}
}
