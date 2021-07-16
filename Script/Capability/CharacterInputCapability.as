
class UCharacterInputCapability : USecCapability
{
	default TickOrder = -10;
	default Tags.Add(CapabilityTags::Input);
	
	USecInputComponent InputComp;
	
	UFUNCTION(BlueprintOverride)
	void Setup()
	{
		InputComp = USecInputComponent::Get(Owner);
	}

    UFUNCTION(BlueprintOverride)
    ECapabilityActivation ShouldActivate() const
    {
        if(!IsLocallyControlled())
			return ECapabilityActivation::DontActivate;

		if(!InputComp.bInitialized)
			return ECapabilityActivation::DontActivate;
		
		return ECapabilityActivation::Local;
    }

    UFUNCTION(BlueprintOverride)
    ECapabilityDeactivation ShouldDeactivate() const
    {
        return ECapabilityDeactivation::DontDeactivate;
    }

    UFUNCTION(BlueprintOverride)
    void OnDeactivate(FCapabilityDeactivationParams& DeactivationParams)
    {
		for(auto It : InputComp.InputAction)
		{
			SetActionState(It.Key, ESecActionState::Inactive);
		}
    }

	UFUNCTION(BlueprintOverride)
	void TickActive(float DeltaTime)
	{
		for(auto It : InputComp.InputAction)
		{
			SetActionState(It.Key, It.Value);
		}

		float X = 0.0f;
		float Y = 0.0f;

		if(InputComp.InputAxis.Contains(SecAxisInput::MoveRight))
			X = InputComp.InputAxis[SecAxisInput::MoveRight];
		if(InputComp.InputAxis.Contains(SecAxisInput::MoveForward))
			Y = InputComp.InputAxis[SecAxisInput::MoveForward];

		const FVector LeftAxisRaw = FVector(X, Y, 0);
		Attribute.SetVectorAttribute(GameplayAction::LeftAxisRaw, LeftAxisRaw);

		X = 0.0f;
		Y = 0.0f;

		if(InputComp.InputAxis.Contains(SecAxisInput::LookRight))
			X = InputComp.InputAxis[SecAxisInput::LookRight];
		if(InputComp.InputAxis.Contains(SecAxisInput::LookUp))
			Y = InputComp.InputAxis[SecAxisInput::LookUp];

		//X = InputComp.InputAxis[SecAxisInput::LookRight];
		//Y = InputComp.InputAxis[SecAxisInput::LookUp];

		const FVector RightAxisRaw = FVector(X, Y, 0);
		Attribute.SetVectorAttribute(GameplayAction::RightAxisRaw, RightAxisRaw);
	}
}
