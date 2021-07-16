
class ASEThirdPersonCamera : ASecThirdPersonCameraBase
{
    UPROPERTY(DefaultComponent, RootComponent)
    USceneComponent Root;

    UPROPERTY(DefaultComponent, Attach = Root)
    USpringArmComponent SpringArm;
    default SpringArm.RelativeRotation = FRotator(-45.0f, 0.0f, 0.0f);
    default SpringArm.TargetArmLength = 630.0f;
    default SpringArm.bDoCollisionTest = false;

    UPROPERTY(DefaultComponent, Attach = SpringARm)
    UCameraComponent CameraComponent;

    UFUNCTION(BlueprintOverride)
    void Tick(float DeltaTime)
    {
        SetActorLocation(GetTargetLocation());
    }


    FVector GetTargetLocation() const
    {
        if(TargetActor != nullptr)
        {
            return TargetActor.GetActorLocation();
        }

        return Root.GetWorldLocation();
    }
}
