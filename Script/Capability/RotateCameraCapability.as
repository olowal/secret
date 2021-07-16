import Character.PlayerCharacter;

class URotateCameraCapability : USecCapability
{
    APlayerCharacter OwnerPlayer;

    UFUNCTION(BlueprintOverride)
    void Setup()
    {
        OwnerPlayer = Cast<APlayerCharacter>(Owner);
    }

    UFUNCTION(BlueprintOverride)
    ECapabilityActivation ShouldActivate() const
    {
        return ECapabilityActivation::Local;
    }

    UFUNCTION(BlueprintOverride)
    ECapabilityDeactivation ShouldDeactivate() const
    {
        return ECapabilityDeactivation::DontDeactivate;
    }

    UFUNCTION(BlueprintOverride)
    void TickActive(float DeltaTime)
    {
        const bool bRotateCameraLeft = Attribute.GetActionAttribute(GameplayAction::RotateCameraLeft);
        const bool bRotateCameraRight = Attribute.GetActionAttribute(GameplayAction::RotateCameraRight);

        if(bRotateCameraLeft)
        {
            OwnerPlayer.SpringArm.AddRelativeRotation(FRotator(0.0f, 1.0f, 0.0f));
        }
        else if(bRotateCameraRight)
        {
            OwnerPlayer.SpringArm.AddRelativeRotation(FRotator(0.0f, -1.0f, 0.0f));
        }
    }
}
