
/*
	Define what relation this actor has with AIs
*/

class UAITeamComponent : USecAITeamComponent
{
	UFUNCTION(BlueprintOverride)
	ETeamAttitude GetTeamAttituteTowards(const AActor Other) const
	{
		return ETeamAttitude::Hostile;
	}
}
