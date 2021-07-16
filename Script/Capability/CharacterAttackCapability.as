import Attack.CharacterAttackComponent;
import Equipment.MeleeWeaponComponent;

class UCharacterAttackCapability : USecCapability
{
    default Tags.Add(n"CharacterAttack");

    default TickOrder = 0;

    ASecCharacter OwnerCharacter;
    UCharacterMovementComponent MoveComp;
    UCharacterAttackComponent AttackComp;
	UMeleeWeaponComponent WeaponComp;

    float CurrentTime = 0.0f;

    int CurrentIndex = 0;

	float StartDelay = 0.0f;

    bool bEndAttackAction = false;
    bool bStartNextAttack = false;
    bool bStartTimer = false;

    float Elapsed = 0.0;

    UFUNCTION(BlueprintOverride)
    void Setup()
    {
        OwnerCharacter = Cast<ASecCharacter>(Owner);
        MoveComp = UCharacterMovementComponent::Get(Owner);
        AttackComp = UCharacterAttackComponent::Get(Owner);
		WeaponComp = UMeleeWeaponComponent::Get(Owner);
    }

    UFUNCTION(BlueprintOverride)
    ECapabilityActivation ShouldActivate() const
    {
        if(!IsLocallyControlled())
			return ECapabilityActivation::DontActivate;

		if(!MoveComp.IsMovingOnGround())
            return ECapabilityActivation::DontActivate;

        if(!WeaponComp.bIsEquipped)
            return ECapabilityActivation::DontActivate;

        if(!WasActionActivated(GameplayAction::Attack))
            return ECapabilityActivation::DontActivate;

        return ECapabilityActivation::Network;
    }

    UFUNCTION(BlueprintOverride)
    ECapabilityDeactivation ShouldDeactivate() const
    {
        if(Elapsed < 0.0f)
            return ECapabilityDeactivation::Local;

        return ECapabilityDeactivation::DontDeactivate;
    }

    UFUNCTION(BlueprintOverride)
    void OnActivate(FCapabilityActivationParams ActivationParams)
    {
        if(WeaponComp.WeaponSettings != nullptr && 
        WeaponComp.WeaponSettings.AttackAnim != nullptr)
        {
            OwnerCharacter.PlayAnimMontage( WeaponComp.WeaponSettings.AttackAnim);
        }

        Elapsed = 0.5f;
    }

    UFUNCTION(BlueprintOverride)
    void OnDeactivate(FCapabilityDeactivationParams& DeactivationParams)
    {
        //Attribute.SetActionAttribute(n"CharacterAttackHold", ESecActionState::Inactive);
    }

    UFUNCTION(BlueprintOverride)
    void TickActive(float DeltaTime)
    {
        Elapsed -= DeltaTime;
    }

    void AttackStart()
    {
        SetAttackInfoAttribute();
        Attribute.SetActionAttribute(n"CharacterAttackHold", ESecActionState::Active);
        bStartTimer = false;
        bEndAttackAction = false;
        bStartNextAttack = false;
    }

    void AttackRelease()
    {
        Attribute.SetActionAttribute(n"CharacterAttackRelease", ESecActionState::ActiveForOneFrame);
        Attribute.SetActionAttribute(n"CharacterAttackHold", ESecActionState::Inactive);
        CurrentTime = GetAttackInfo().AttackDuration;
        bStartTimer = true;
    }

    bool IncrementAttackIndex()
    {
        if((CurrentIndex + 1) < WeaponComp.GetIndexTotal())
        {
            CurrentIndex++;
            return true;
        }

        return false;
    }

    UMeleeWeaponAttackSettings GetAttackInfo() const
    {
        return WeaponComp.WeaponSettings.AttackInfo[CurrentIndex];
    }

    void SetAttackInfoAttribute()
    {
        //AttackComp.AttackInfo = AttackInfo;
    }
}
