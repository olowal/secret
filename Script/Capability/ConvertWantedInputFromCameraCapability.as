import Character.PlayerCharacter;
import Tags.CharacterTags;

class UConvertWantedInputFromCameraAction : USecCapability
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
        FVector RawInput = Attribute.GetVectorAttribute(GameplayAction::LeftAxisRaw);

        UCameraComponent CameraComponent = OwnerPlayer.CameraComponent;

        const FVector CameraUp = CameraComponent.UpVector;
        const FVector CameraRight = CameraComponent.RightVector;
        const FVector CameraForward = CameraComponent.ForwardVector;

        const FVector Up = CameraUp * RawInput.Y;
        const FVector Right = CameraRight * RawInput.X;

        const FVector CameraProject = (Up + Right).GetSafeNormal().VectorPlaneProject(FVector::UpVector) * RawInput.Size();
        //System::DrawDebugArrow(Owner.ActorLocation, Owner.ActorLocation + CameraProject * 300.0f, 5.0f, FLinearColor::Green, DeltaTime, 3.0f);
        Attribute.SetVectorAttribute(CharacterTags::MovementDirection, CameraProject);
    }

}