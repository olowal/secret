#include "SecAITeamComponent.h"

ETeamAttitude::Type USecAITeamComponent::GetTeamAttitudeTowards(const AActor& Other) const
{
	return BP_GetTeamAttituteTowards(&Other);
}

ETeamAttitude::Type USecAITeamComponent::BP_GetTeamAttituteTowards_Implementation(const AActor* Other) const
{
	return ETeamAttitude::Hostile;
}
