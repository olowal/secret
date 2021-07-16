import AI.Perception.PerceptionTargetInfo;

/*
	Keeps stored data of current target. Get's last known location.
*/

class UAITargetingComponent : UActorComponent
{
	const UPerceptionTargetInfo CurrentTarget = nullptr;

	bool HasTarget() const
	{
		return CurrentTarget != nullptr;
	}

	bool IsVisible() const
	{
		if(CurrentTarget == nullptr)
			return false;

		return CurrentTarget.bVisible;
	}

	FVector GetTargetLocation() const property
	{
		if(CurrentTarget == nullptr)
			return FVector::ZeroVector;

		if(IsVisible())
			return CurrentTarget.Actor.ActorLocation;

		return CurrentTarget.LastLocation;
	}

	AActor GetTargetActor() const property
	{
		if(CurrentTarget == nullptr)
			return nullptr;

		return CurrentTarget.Actor;
	}
}
