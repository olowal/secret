import Components.CharacterAnimationComponent;

class SEAnimInstance : UAnimInstance
{
    UPROPERTY(BlueprintReadOnly)
    float HorizontalMovement = 0.0f;

    UPROPERTY(BlueprintReadOnly)
    float VerticalMovement = 0.0f;

    UPROPERTY(BlueprintReadOnly)
    float Direction = 0.0f;

    UPROPERTY(BlueprintReadOnly)
    bool bIsBlocking = false;

    UPROPERTY(BlueprintReadOnly)
    bool bIsSprinting = false;

    UPROPERTY(BlueprintReadOnly)
    bool bInAir = false;

    UCharacterAnimationComponent CharacterAnimComp;

	UFUNCTION(BlueprintOverride)
	void BlueprintInitializeAnimation()
	{
        CharacterAnimComp = UCharacterAnimationComponent::Get(TryGetPawnOwner());
	}

    UFUNCTION(BlueprintOverride)
    void BlueprintUpdateAnimation(float DeltaTimeX)
    {
        if(CharacterAnimComp == nullptr)
            return;

        HorizontalMovement = CharacterAnimComp.HorizontalMovement;
        VerticalMovement = CharacterAnimComp.VerticalMovement;
        Direction = CharacterAnimComp.Direction;

        bIsBlocking = CharacterAnimComp.bIsBlocking;
        bIsSprinting = CharacterAnimComp.bIsSprinting;
        bInAir = CharacterAnimComp.bInAir;
    }
}
