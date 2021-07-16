#include "SecAIController.h"
#include "AI/Team/SecAITeamComponent.h"
#include "AI/CombatArea/SecCombatAreaComponent.h"

ASecAIController::ASecAIController(const FObjectInitializer& ObjectInitializer)
{
	CombatAreaComponent = ObjectInitializer.CreateDefaultSubobject<USecCombatAreaComponent>(this, TEXT("CombatAreaComponent"));
}

ETeamAttitude::Type ASecAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
	if (AITeamComponent != nullptr)
	{
		return AITeamComponent->GetTeamAttitudeTowards(Other);
	}

	return ETeamAttitude::Hostile;
}

void ASecAIController::PostRegisterAllComponents()
{
	Super::PostRegisterAllComponents();

	if (AITeamComponent == nullptr)
	{
		AITeamComponent = FindComponentByClass<USecAITeamComponent>();
	}
}

bool ASecAIController::ValidateMoveToLocation(FVector& MoveToLocation) const
{
	return CombatAreaComponent->ValidateLocation(MoveToLocation);
}

void ASecAIController::SetMovementEnabled(bool bValue)
{
	bMovementEnabled = bValue;

	if (!bMovementEnabled)
		StopMovement();
}

FPathFollowingRequestResult ASecAIController::MoveTo(const FAIMoveRequest& MoveRequest, FNavPathSharedPtr* OutPath)
{
	if (!IsMovementEnabled())
	{
		FPathFollowingRequestResult ResultData;
		ResultData.Code = EPathFollowingRequestResult::Failed;
		return ResultData;
	}

	return Super::MoveTo(MoveRequest, OutPath);
}
