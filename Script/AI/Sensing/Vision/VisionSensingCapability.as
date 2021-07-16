import AI.Sensing.Vision.VisionSensingComponent;

class UVisionSensingCapability : USecCapability
{
	UVisionSensingComponent Vision;

	UFUNCTION(BlueprintOverride)
	void Setup()
	{
		Vision = UVisionSensingComponent::Get(Owner);
	}

	UFUNCTION(BlueprintOverride)
	ECapabilityActivation ShouldActivate() const
	{
		if(!HasAuthority())
			return ECapabilityActivation::DontActivate;

		if(!Vision.bRunVisionSensing)
			return ECapabilityActivation::DontActivate;

		if(Vision.SensingSettings == nullptr)
			return ECapabilityActivation::DontActivate;

		return ECapabilityActivation::Local;
	}

	UFUNCTION(BlueprintOverride)
	void TickActive(float DeltaTime)
	{

	}

	UFUNCTION(BlueprintOverride)
	ECapabilityDeactivation ShouldDeactivate() const
	{
		if(!Vision.bRunVisionSensing)
			return ECapabilityDeactivation::Local;

		if(Vision.SensingSettings == nullptr)
			return ECapabilityDeactivation::Local;

		return ECapabilityDeactivation::DontDeactivate;
	}
}
