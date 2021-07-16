
class UAIMovementCapability : USecCapability
{
	default Tags.Add(n"CharacterMovement");

	APawn OwnerPawn;

	UFUNCTION(BlueprintOverride)
	void Setup()
	{
		OwnerPawn = Cast<APawn>(Owner);
	}

	UFUNCTION(BlueprintOverride)
	ECapabilityActivation ShouldActivate() const
	{
		return ECapabilityActivation::Network;
	}

	UFUNCTION(BlueprintOverride)
	void OnActivate(FCapabilityActivationParams ActivationParams)
	{
		ASecAIController Controller = Cast<ASecAIController>(OwnerPawn.GetController());

		if(Controller != nullptr)
			Controller.SetMovementEnabled(true);
	}

	UFUNCTION(BlueprintOverride)
	ECapabilityDeactivation ShouldDeactivate() const
	{
		return ECapabilityDeactivation::DontDeactivate;
	}

	UFUNCTION(BlueprintOverride)
	void OnDeactivate(FCapabilityDeactivationParams DeactivationParams)
	{
		ASecAIController Controller = Cast<ASecAIController>(OwnerPawn.GetController());

		if(Controller != nullptr)
			Controller.SetMovementEnabled(false);
	}
}
