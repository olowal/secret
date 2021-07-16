/*
	Set face direction to the direction from owner towards mouse location.
*/

class UFaceDirectionToMouseLocationCapability : USecCapability
{
	default TickOrder = -1;

	APawn OwnerPawn = nullptr;
	USecCharacterMovementComponent MoveComp = nullptr;

	UFUNCTION(BlueprintOverride)
	void Setup()
	{
		OwnerPawn = Cast<APawn>(Owner);
		MoveComp = USecCharacterMovementComponent::Get(Owner);
	}

    UFUNCTION(BlueprintOverride)
    ECapabilityActivation ShouldActivate() const
	{
		if(!IsLocallyControlled())
			return ECapabilityActivation::DontActivate;

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
		APlayerController PlayerController = Cast<APlayerController>(OwnerPawn.Controller);

		// Because the player may not be possessed upon activation.
		if(PlayerController == nullptr)
			return;

		float X = 0, Y = 0;	
		PlayerController.GetMousePosition(X, Y);
		FVector2D MousePos(X, Y);
		
		if(MousePos.IsNearlyZero())
			return;

		FVector2D OwnerScreenLocation;
		PlayerController.ProjectWorldLocationToScreen(Owner.ActorLocation, OwnerScreenLocation);
		const FVector2D OwnerToMousePos = (MousePos - OwnerScreenLocation);

		// Rotate 90 degrees because of mesh being rotated (Please fix this)
		FVector FacingDirection = FVector(OwnerToMousePos.X, OwnerToMousePos.Y, 0).GetSafeNormal2D().RotateAngleAxis(90.0f, FVector::UpVector);
		MoveComp.SetTargetFacingDirection(FacingDirection);
	}
}
