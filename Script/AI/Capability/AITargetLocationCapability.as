import AI.Perception.AITargetingComponent;

/*
	Updates the blackboard with the location of the current target.
	Placed on the ai-controller.

	Update: This actually updates the blackboard with details from the current target.
*/

class UAITargetLocationCapability : USecCapability
{
	UBlackboardComponent Blackboard;
	UAITargetingComponent TargetComp;
	UCharacterMovementComponent MoveComp;

	APawn ControlledPawn = nullptr;
	USecAttributeComponent ControlledPawnAttribute;

	AActor LastTargetActor = nullptr;
	bool bWasTargetVisible = false;
	bool bHadTarget = false;

	UFUNCTION(BlueprintOverride)
	void Setup()
	{
		Blackboard = UBlackboardComponent::Get(Owner);
		TargetComp = UAITargetingComponent::Get(Owner);

		AController Controller = Cast<AController>(Owner);
		ControlledPawn = Controller.ControlledPawn;
		MoveComp = UCharacterMovementComponent::Get(ControlledPawn);
		ControlledPawnAttribute = USecAttributeComponent::Get(ControlledPawn);
	}

	UFUNCTION(BlueprintOverride)
	ECapabilityActivation ShouldActivate() const
	{
		return ECapabilityActivation::Network;
	}

	UFUNCTION(BlueprintOverride)
	void TickActive(float DeltaTime)
	{
		AActor TargetActor = TargetComp.TargetActor;
		bool bHasTarget = TargetComp.HasTarget();
		bool bIsTargetVisible = TargetComp.IsVisible();

		if(TargetActor != LastTargetActor)
		{
			Blackboard.SetValueAsObject(n"TargetActor", TargetActor);
		}

		if(bHasTarget != bHadTarget)
		{
			Blackboard.SetValueAsBool(n"HasTarget", bHasTarget);
		}

		if(bIsTargetVisible != bWasTargetVisible)
		{
			Blackboard.SetValueAsBool(n"IsTargetVisible", bIsTargetVisible);
		}
		
		if(bHasTarget)
		{
			const FVector TargetLocation = TargetComp.TargetLocation;
			Blackboard.SetValueAsVector(n"TargetLocation", TargetLocation);
			FVector DirectionToTarget = (TargetLocation - ControlledPawn.ActorLocation).GetSafeNormal2D();
			ControlledPawnAttribute.SetVectorAttribute(n"CharacterMovementDirection", DirectionToTarget);
		}

		System::DrawDebugSphere(TargetComp.TargetLocation, 20, 12, FLinearColor::Green);

		bHadTarget = bHasTarget;
		bWasTargetVisible = bIsTargetVisible;
		LastTargetActor = TargetActor;
	}

    UFUNCTION(BlueprintOverride)
    ECapabilityDeactivation ShouldDeactivate() const
    {
		return ECapabilityDeactivation::DontDeactivate;
    }
}
