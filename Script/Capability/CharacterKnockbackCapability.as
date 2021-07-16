import Components.KnockbackComponent;

class UCharacterKnockbackCapability : USecCapability
{
	default TickOrder = 9;

	UKnockbackComponent Knockback;
	USecCharacterMovementComponent MoveComp;
	float Elapsed = 0;

	UFUNCTION(BlueprintOverride)
	void Setup()
	{
		Knockback = UKnockbackComponent::Get(Owner);
		MoveComp = USecCharacterMovementComponent::Get(Owner);
	}

    UFUNCTION(BlueprintOverride)
    ECapabilityActivation ShouldActivate() const
    {
        if(!Knockback.bKnockback)
			return ECapabilityActivation::DontActivate;

        return ECapabilityActivation::Local;
    }

    UFUNCTION(BlueprintOverride)
    void OnActivate(FCapabilityActivationParams ActivationParams)
	{
        BlockCapabilities(n"CharacterMovement", this);
        BlockCapabilities(n"CharacterAttack", this);
        BlockCapabilities(n"CharacterBlock", this);

		MoveComp.ApplyKnockback(Knockback.Direction * 1000);
		Elapsed = 0.6f;
	}

    UFUNCTION(BlueprintOverride)
    ECapabilityDeactivation ShouldDeactivate() const
    {
        if(Elapsed <= 0.0f)
            return ECapabilityDeactivation::Local;

        return ECapabilityDeactivation::DontDeactivate;
    }

    UFUNCTION(BlueprintOverride)
    void OnDeactivate(FCapabilityDeactivationParams& DeactivationParams)
    {
        UnblockCapabilities(n"CharacterMovement", this);
        UnblockCapabilities(n"CharacterAttack", this);
        UnblockCapabilities(n"CharacterBlock", this);
		Knockback.bKnockback = false;
    }

	UFUNCTION(BlueprintOverride)
	void TickActive(float DeltaTime)
	{
		Elapsed -= DeltaTime;
	}
}
