import AI.Sensing.Vision.VisionSensingSettings;

class UVisionSensingComponent : UActorComponent
{
	UFUNCTION(BlueprintOverride)
	void BeginPlay()
	{
		Owner.AddCapability(n"VisionSensingCapability");
	}

	private USceneComponent _OriginComponent = nullptr;
	
	FVector GetOriginWorldLocation() const
	{
		return (_OriginComponent != nullptr ? _OriginComponent.WorldLocation : Owner.ActorLocation);
	}

	void SetOriginComponent(USceneComponent InOriginComponent) { _OriginComponent = InOriginComponent; }

	private UVisionSensingSettings _VisionSensingSettings = nullptr;

	void SetSensingSettings(UVisionSensingSettings InSettings) { _VisionSensingSettings = InSettings; }
	UVisionSensingSettings GetSensingSettings() const property { return _VisionSensingSettings; }

	bool bRunVisionSensing = true;

}
