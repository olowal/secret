
/*
	Contains data on detected targets. Objects are cached, reused and never removed so it can be acessed elsewhere.
*/

class UPerceptionTargetInfo : UObject
{
	AActor Actor = nullptr;
	FVector LastLocation = FVector::ZeroVector;
	float Aggro = 0;
	bool bVisible = false;

	FVector GetTargetLocation() const
	{
		if(bVisible && Actor != nullptr)
			return Actor.ActorLocation;

		return LastLocation;
	}
}
