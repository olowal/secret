import Components.CharacterAnimationComponent;
import Equipment.ShieldComponent;
import Components.SprintComponent;

class UCharacterAnimationCapability : USecCapability
{
    USkeletalMeshComponent SkeletalMeshComp;
    UCharacterMovementComponent MoveComp;
    UCharacterAnimationComponent CharacterAnimComp;
    UShieldComponent ShieldComp;
    USprintComponent SprintComp;

    UFUNCTION(BlueprintOverride)
    void Setup()
    {
        SkeletalMeshComp = USkeletalMeshComponent::Get(Owner);
        CharacterAnimComp = UCharacterAnimationComponent::Get(Owner);
        MoveComp = UCharacterMovementComponent::Get(Owner);
        ShieldComp = UShieldComponent::Get(Owner);
        SprintComp = USprintComponent::Get(Owner);
    }

    UFUNCTION(BlueprintOverride)
    ECapabilityActivation ShouldActivate() const
    {
        if(!CharacterAnimComp.CharacterAnimInstanceClass.IsValid())
            return ECapabilityActivation::DontActivate;
        
        return ECapabilityActivation::Local;
    }

    UFUNCTION(BlueprintOverride)
    void OnActivate(FCapabilityActivationParams ActivationParams)
    {
        // Clear the old one just to be safe
        SkeletalMeshComp.SetAnimClass(nullptr);
        SkeletalMeshComp.SetAnimClass(CharacterAnimComp.CharacterAnimInstanceClass);
    }

    UFUNCTION(BlueprintOverride)
    void TickActive(float DeltaTime)
    {
        const FVector Velocity = MoveComp.Velocity;
        CharacterAnimComp.VerticalMovement = Velocity.Z;
        
        FRotator DeltaRotation = Owner.ActorRotation - Velocity.ToOrientationRotator();
        DeltaRotation.Normalize();
        CharacterAnimComp.Direction = -DeltaRotation.Yaw;
        CharacterAnimComp.HorizontalMovement = Velocity.Size2D();
		
        CharacterAnimComp.bIsBlocking = ShieldComp.bIsBlocking;
        CharacterAnimComp.bIsSprinting = SprintComp.bIsSprinting;
        CharacterAnimComp.bInAir = !MoveComp.IsMovingOnGround();
    }

}
