
class UShieldBlockDamageModifier : USecDamageModifier
{
	UFUNCTION(BlueprintOverride)
	void Setup()
	{
		
	}

	UFUNCTION(BlueprintOverride)
	void OnApply()
	{

	}

	UFUNCTION(BlueprintOverride)
	void ProcessDamageOutput(FSecDamageInput Input, FSecDamageOutput& Output) const
	{
        const float BlockingAngle = 100.0f;
        const float BlockingRadius = FMath::Cos(FMath::DegreesToRadians(BlockingAngle * 0.5f));
		const FVector DirectionToOrigin = (Input.DamageOrigin - Owner.ActorLocation).GetSafeNormal();
        const float DirectionDot = DirectionToOrigin.DotProduct(Owner.ActorForwardVector);

		if(DirectionDot > BlockingRadius)
		{
			Output.DamageDealt = 0;
		}
	}
}
