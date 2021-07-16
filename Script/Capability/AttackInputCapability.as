
class UAttackInputCapability : USecCapability
{
	default TickOrder = 1;

	USecInputComponent InputComp;

	UFUNCTION(BlueprintOverride)
	void Setup()
	{
		InputComp = USecInputComponent::Get(Owner);
	}

	UFUNCTION(BlueprintOverride)
	ECapabilityActivation ShouldActivate() const
	{
		if(!InputComp.bInitialized)
			return ECapabilityActivation::DontActivate;

		return ECapabilityActivation::Network;
	}

	UFUNCTION(BlueprintOverride)
	void TickActive(float DeltaTime)
	{
		ESecActionState AttackActionState = InputComp.InputAction[GameplayAction::Attack];
		Attribute.SetActionAttribute(GameplayAction::Attack, AttackActionState);
	}

	UFUNCTION(BlueprintOverride)
	ECapabilityDeactivation ShouldDeactivate() const
	{
		return ECapabilityDeactivation::DontDeactivate;
	}
}
