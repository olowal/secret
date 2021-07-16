import Tags.CharacterTags;

class UCharacterMoveOnFloorCapability : USecCapability
{
    default Tags.Add(CapabilityTags::CharacterMovement);

    ASecCharacter OwnerCharacter;

    UFUNCTION(BlueprintOverride)
    ECapabilityActivation ShouldActivate() const
    {
        return ECapabilityActivation::Local; // CharacterMovementComponent replicates this behavior
    }

    UFUNCTION(BlueprintOverride)
    ECapabilityDeactivation ShouldDeactivate() const
    {
        return ECapabilityDeactivation::DontDeactivate;
    }

    UFUNCTION(BlueprintOverride)
    void Setup()
    {
        OwnerCharacter = Cast<ASecCharacter>(Owner);
    }

    UFUNCTION(BlueprintOverride)
    void TickActive(float DeltaTime)
    {
        const FVector MovementDirection = Attribute.GetVectorAttribute(CharacterTags::MovementDirection);
        OwnerCharacter.AddMovementInput(MovementDirection, 1.0f);
/*
		float t = 10.23f;
		float max = 50.0f;
		float bit = 255.0f;
		PrintToScreen("original " + t);
		int8 compressed = FMath::RoundToInt(t * bit / max);
		PrintToScreen("compressed " + compressed);

		float uncompressed = float(compressed) * max / bit;
		PrintToScreen("uncompressed " + uncompressed);
*/
		int Turn = FMath::Rand() % 10;
		//PrintToScreen("Turn " + Turn);
    }
}
