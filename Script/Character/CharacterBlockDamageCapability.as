import Components.TakeDamageComponent;

class UCharacterBlockDamageCapability : USecCapability
{
	default TickOrder = 9;

	UTakeDamageComponent TakeDamageComp;

	UFUNCTION(BlueprintOverride)
	void Setup()
	{
		TakeDamageComp = UTakeDamageComponent::Get(Owner);
	}

    UFUNCTION(BlueprintOverride)
    ECapabilityActivation ShouldActivate() const
    {
        if(!Attribute.GetActionAttribute(n"IsBlocking"))
            return ECapabilityActivation::DontActivate;

		if(!TakeDamageComp.HasPendingDamage())
			return ECapabilityActivation::DontActivate;

		if(!IsFacingDamage())
			return ECapabilityActivation::DontActivate;

        return ECapabilityActivation::DontActivate;
    }

	UFUNCTION(BlueprintOverride)
	void OnActivate(FCapabilityActivationParams ActivationParams)
	{
		for(int Index = TakeDamageComp.CumulatedDamage.Num() - 1; Index >= 0; --Index)
		{
			FTakeDamageInfo& DamageInfo = TakeDamageComp.CumulatedDamage[Index];

			if(IsBlockValid(DamageInfo.DirectionToDamage))
				TakeDamageComp.CumulatedDamage.RemoveAt(Index);
		}
	}

	UFUNCTION(BlueprintOverride)
	ECapabilityDeactivation ShouldDeactivate() const
	{
        if(!Attribute.GetActionAttribute(n"IsBlocking"))
            return ECapabilityDeactivation::Local;

		if(!TakeDamageComp.HasPendingDamage())
			return ECapabilityDeactivation::Local;

		return ECapabilityDeactivation::DontDeactivate;
	}

	// check if at least one is facing the direction of the incomming damage
	private bool IsFacingDamage() const
	{
		for(FTakeDamageInfo DamageInfo : TakeDamageComp.CumulatedDamage)
		{
			if(IsBlockValid(DamageInfo.DirectionToDamage))
				return true;
		}

		return false;
	}

    private bool IsBlockValid(FVector DirectionToOrigin) const
    {
        const float BlockingAngle = 100.0f;
        const float BlockingRadius = FMath::Cos(FMath::DegreesToRadians(BlockingAngle * 0.5f));
        const float DirectionDot = DirectionToOrigin.DotProduct(Owner.ActorForwardVector);

        return DirectionDot > BlockingRadius;
    }
}
