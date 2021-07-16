import Equipment.MeleeWeaponComponent;

class UCharacterAttackHoldCapability : USecCapability
{
    default Tags.Add(n"CharacterAttack");

    ASecCharacter OwnerCharacter;
	UMeleeWeaponComponent WeaponComp;

    float HoldTime = 0.0f;
	float Elapsed = 0.0f;

    default TickOrder = 100;

    UFUNCTION(BlueprintOverride)
    void Setup()
    {
        OwnerCharacter = Cast<ASecCharacter>(Owner);
        WeaponComp = UMeleeWeaponComponent::Get(Owner);
    }

    UFUNCTION(BlueprintOverride)
    ECapabilityActivation ShouldActivate() const
    {
        if(WeaponComp.WeaponSettings == nullptr)
            return ECapabilityActivation::DontActivate;

		if(!IsAction(GameplayAction::Attack))
			return ECapabilityActivation::DontActivate;

		if(WeaponComp.AttackState != EMeleeWeaponAttackState::None)
			return ECapabilityActivation::DontActivate;

        return ECapabilityActivation::Network;
    }

	UFUNCTION(BlueprintOverride)
	bool RemoteShouldActivate() const
	{
		return WeaponComp.WeaponSettings != nullptr;
	}

    UFUNCTION(BlueprintOverride)
    void PreActivation(FActivationSyncParams& SyncParams)
    {
		const UAnimMontage CurrentMontage = OwnerCharacter.CurrentMontage;
		if(WeaponComp.LastPlayedMontage != nullptr && WeaponComp.LastPlayedMontage == CurrentMontage)
		{
			WeaponComp.IncrementAttackIndex();
		}
		else
		{
			WeaponComp.ResetAttackIndex();
		}

		UAnimMontage AttackMontage = WeaponComp.GetMontageCurrent();
		SyncParams.AddObject(n"AttackMontage", AttackMontage);
    }

    UFUNCTION(BlueprintOverride)
    void OnActivate(FCapabilityActivationParams ActivationParams)
    {
        WeaponComp.NumHold++;
		UAnimMontage MontageCurrent = Cast<UAnimMontage>(ActivationParams.GetObject(n"AttackMontage"));

        if(MontageCurrent == nullptr)
            return;

		WeaponComp.LastPlayedMontage = MontageCurrent;
		WeaponComp.AttackMontageCurrent = MontageCurrent;
		float SectionLength = MontageCurrent.GetSectionLengthByName(n"Start");
		float Mul = 1.0f;
		Elapsed = (SectionLength / Mul);
		OwnerCharacter.PlayAnimMontage(MontageCurrent, Mul, n"Start");
        HoldTime = 0.0f;
		WeaponComp.AttackState = EMeleeWeaponAttackState::Hold;

        BlockCapabilities(n"CharacterBlock", this);
    }

    UFUNCTION(BlueprintOverride)
    ECapabilityDeactivation ShouldDeactivate() const
    {
        if(Elapsed > 0.0f)
			return ECapabilityDeactivation::DontDeactivate;

		if(!IsAction(GameplayAction::Attack))
			return ECapabilityDeactivation::Network;

        return ECapabilityDeactivation::DontDeactivate;
    }

	UFUNCTION(BlueprintOverride)
	void PreDeactivation(FDeactivationSyncParams& SyncParams)
	{
		// If this is still true when we deactivated, we can assume this capability was interrupted.
		if(WeaponComp.bWantsToAttack)
			SyncParams.AddActionState(n"Interrupted");
	}

    UFUNCTION(BlueprintOverride)
    void OnDeactivate(FCapabilityDeactivationParams DeactivationParams)
    {
		WeaponComp.AttackState = EMeleeWeaponAttackState::Release;
		
		UnblockCapabilities(n"CharacterBlock", this);
    }

    UFUNCTION(BlueprintOverride)
    void TickActive(float DeltaTime)
    {
        HoldTime += DeltaTime;
		Elapsed -= DeltaTime;
    }

	UFUNCTION(BlueprintOverride)
	void PreTick(float DeltaTime)
	{
		//PrintToScreen("Num Crumbs: " + NumCrumbs);
	}
}
