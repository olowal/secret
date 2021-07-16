#pragma once

#include "AIController.h"
#include "SecAIController.generated.h"

class USecAITeamComponent;
class USecCombatAreaComponent;

UCLASS()
class SECRET_API ASecAIController : public AAIController
{
	GENERATED_BODY()
public:
	ASecAIController(const FObjectInitializer& ObjectInitializer);

	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;

	virtual void PostRegisterAllComponents() override;

	bool ValidateMoveToLocation(FVector& MoveToLocation) const;

	UFUNCTION(BlueprintCallable, Category = AI)
	void SetMovementEnabled(bool bValue);
	UFUNCTION(BlueprintPure, Category = AI)
	bool IsMovementEnabled() const { return bMovementEnabled; }

	virtual FPathFollowingRequestResult MoveTo(const FAIMoveRequest& MoveRequest, FNavPathSharedPtr* OutPath = nullptr) override;

protected:
	bool bMovementEnabled = true;

	UPROPERTY(VisibleDefaultsOnly, Category = CombatArea)
	USecCombatAreaComponent* CombatAreaComponent = nullptr;

	UPROPERTY(Transient)
	USecAITeamComponent* AITeamComponent = nullptr;
};
