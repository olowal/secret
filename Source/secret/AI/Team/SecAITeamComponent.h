#pragma once

#include "Components/ActorComponent.h"
#include "AIModule/Classes/GenericTeamAgentInterface.h"
#include "SecAITeamComponent.generated.h"

UCLASS(Abstract)
class SECRET_API USecAITeamComponent : public UActorComponent
{
	GENERATED_BODY()
public:

	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const;

	UFUNCTION(BlueprintNativeEvent, Category = AI, meta = (DisplayName = "Get Team Attitude Towards"))
	ETeamAttitude::Type BP_GetTeamAttituteTowards(const AActor* Other) const;
};
