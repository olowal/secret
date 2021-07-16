

class APlayerCharacter : ASecPlayerCharacter
{
    UPROPERTY(DefaultComponent, Attach = CapsuleComponent)
    USpringArmComponent SpringArm;

    UPROPERTY(DefaultComponent, Attach = SpringArm)
    UCameraComponent CameraComponent;

	default CapsuleComponent.CollisionProfileName = n"Character";
}
