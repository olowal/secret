
class UFaceDirectionCapability : USecCapability
{
    default Tags.Add(n"CharacterMovement");
    default Tags.Add(n"CharacterFaceDirection");

    USecCharacterMovementComponent MoveComp;
    ASecCharacter OwnerCharacter;

    FQuat CurrentRotation;

    UFUNCTION(BlueprintOverride)
    void Setup()
    {
        OwnerCharacter = Cast<ASecCharacter>(Owner);
        MoveComp = USecCharacterMovementComponent::Get(Owner);
    }

    UFUNCTION(BlueprintOverride)
    ECapabilityActivation ShouldActivate() const
    {
        if(!IsLocallyControlled())
			return ECapabilityActivation::DontActivate;

		if(!MoveComp.IsMovingOnGround())
            return ECapabilityActivation::DontActivate;
        
        return ECapabilityActivation::Local;
    }

    UFUNCTION(BlueprintOverride)
    ECapabilityDeactivation ShouldDeactivate() const
    {
        if(!MoveComp.IsMovingOnGround())
        {
            return ECapabilityDeactivation::Local;
        }

        return ECapabilityDeactivation::DontDeactivate;
    }

    UFUNCTION(BlueprintOverride)
    void TickActive(float DeltaTime)
    {
        if(IsLocallyControlled())
        {
            FVector MovementDirection = Attribute.GetVectorAttribute(n"CharacterMovementDirection");
            
            if(!MovementDirection.IsNearlyZero())
            {
                CurrentRotation = FQuat::Slerp(CurrentRotation, MovementDirection.ToOrientationQuat(), 10.5 * DeltaTime);
                MoveComp.SetTargetFacingRotation(CurrentRotation);
            }
        }
    }
}
