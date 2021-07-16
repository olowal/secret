
class UCharacterAnimationComponent : UActorComponent
{
    default PrimaryComponentTick.bStartWithTickEnabled = false;

    UPROPERTY(EditAnywhere)
    TSubclassOf<UAnimInstance> CharacterAnimInstanceClass;

    float HorizontalMovement = 0.0f;
    float VerticalMovement = 0.0f;
    float Direction = 0.0f;
    bool bIsBlocking = false;
    bool bIsSprinting = false;
    bool bInAir = false;
}
