import Components.HitStaggerComponent;
import Equipment.MeleeWeaponComponent;

class UHitStaggerCapability : USecCapability
{
	UHitStaggerComponent HitStagger;
	UMeleeWeaponComponent WeaponComp;
	ACharacter Character;
	float Elapsed = 0.0f;

	UFUNCTION(BlueprintOverride)
	void Setup()
	{
		Character = Cast<ACharacter>(Owner);
		HitStagger = UHitStaggerComponent::Get(Owner);
		WeaponComp = UMeleeWeaponComponent::Get(Owner);
	}

    UFUNCTION(BlueprintOverride)
    ECapabilityActivation ShouldActivate() const
	{
		if(!HitStagger.bHitStagger)
			return ECapabilityActivation::DontActivate;

		return ECapabilityActivation::Local;
	}

    UFUNCTION(BlueprintOverride)
    void OnActivate(FCapabilityActivationParams ActivationParams)
	{
		BlockCapabilities(n"CharacterMovement", this);
		BlockCapabilities(n"CharacterAttack", this);
		BlockCapabilities(n"CharacterBlock", this);

		Elapsed = HitStagger.HitStaggerTime;
		Character.PlayAnimMontage(HitStagger.HitStaggerAnim, HitStagger.HitStaggerMultiplier);
	}

    UFUNCTION(BlueprintOverride)
    ECapabilityDeactivation ShouldDeactivate() const
	{
		if(Elapsed <= 0)
			return ECapabilityDeactivation::Local;

		return ECapabilityDeactivation::DontDeactivate;
	}

    UFUNCTION(BlueprintOverride)
    void OnDeactivate(FCapabilityDeactivationParams& DeactivationParams)
	{
		UnblockCapabilities(n"CharacterMovement", this);
		UnblockCapabilities(n"CharacterAttack", this);
		UnblockCapabilities(n"CharacterBlock", this);

		HitStagger.bHitStagger = false;
		WeaponComp.ResetAttackIndex();
	}

	UFUNCTION(BlueprintOverride)
	void TickActive(float DeltaTime)
	{
		Elapsed -= DeltaTime;
	}
}
