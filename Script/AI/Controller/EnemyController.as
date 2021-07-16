import AI.Perception.SecAIPerceptionComponent;
import AI.Team.AITeamComponent;
import AI.Perception.AITargetingComponent;

UCLASS(Abstract)
class AEnemyController : ASecAIController
{
	UPROPERTY(DefaultComponent)
	USecAIPerceptionComponent SecPerceptionComponent;

	UPROPERTY(DefaultComponent)
	UAITeamComponent TeamComponent;

	UPROPERTY(DefaultComponent)
	USecCapabilitySystemComponent CapabilitySystem;

	UPROPERTY(DefaultComponent)
	USecAttributeComponent Attribute;

	UPROPERTY(DefaultComponent)
	UAITargetingComponent Targeting;

	UFUNCTION(BlueprintOverride)
	void ReceivePossess(APawn PossessedPawn)
	{
		SecPerceptionComponent.CachedIgnoreActors.AddUnique(PossessedPawn);
		SecPerceptionComponent.SetOriginTransform(PossessedPawn.RootComponent);

		AddCapability(n"AISelectTargetCapability");
		AddCapability(n"AITargetLocationCapability");
		AddCapability(n"VisionSensingCapability");
	}

	UFUNCTION(BlueprintOverride)
	void ReceiveUnPossess(APawn UnpossessedPawn)
	{
		SecPerceptionComponent.CachedIgnoreActors.Remove(UnpossessedPawn);
		SecPerceptionComponent.SetOriginTransform(nullptr);
	}

	UFUNCTION(BlueprintOverride)
	void BeginPlay()
	{

	}
}
