
class UCharacterJumpCapability : USecCapability
{
    default Tags.Add(CapabilityTags::CharacterJump);

    USecCharacterMovementComponent MoveComp;
    ASecCharacter OwnerCharacter;

    UFUNCTION(BlueprintOverride)
    void Setup()
    {
        MoveComp = USecCharacterMovementComponent::Get(Owner);
        OwnerCharacter = Cast<ASecCharacter>(Owner);
        Tags.Add(GameplayAction::CharacterJump);
    }

    UFUNCTION(BlueprintOverride)
    ECapabilityActivation ShouldActivate() const
    {
        if(!MoveComp.IsMovingOnGround())
        {
            return ECapabilityActivation::DontActivate;
        }

        if(!IsActionActivated(GameplayAction::CharacterJump))
        {
            return ECapabilityActivation::DontActivate;
        }
        
        return ECapabilityActivation::Local;
    }

    UFUNCTION(BlueprintOverride)
    ECapabilityDeactivation ShouldDeactivate() const
    {
        return ECapabilityDeactivation::Local;
    }

    UFUNCTION(BlueprintOverride)
    void OnActivate(FCapabilityActivationParams ActivationParams)
    {
        OwnerCharacter.Jump();
    }
}
