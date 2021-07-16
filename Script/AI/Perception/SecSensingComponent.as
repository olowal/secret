import void RegisterSensingComponent(USecSensingComponent Comp) from "GameInstance.ScriptSecGameInstance";
import void UnregisterSensingComponent(USecSensingComponent Comp) from "GameInstance.ScriptSecGameInstance";
/*
	Allows the SecAIPerceptionComponent to detect Actors.
*/

class USecSensingComponent : UActorComponent
{
	// The location on the actor used as the target point to check for visibility. If nullptr, ActorLocation will be used.
	private USceneComponent _VisionCenter = nullptr;

	UFUNCTION(BlueprintOverride)
	void BeginPlay()
	{
		RegisterSensingComponent(this);
	}

	UFUNCTION(BlueprintOverride)
	void EndPlay(EEndPlayReason Reason)
	{
		UnregisterSensingComponent(this);
	}

	FVector GetVisionCenterLocation() const property
	{
		return _VisionCenter == nullptr ? Owner.ActorLocation : _VisionCenter.WorldLocation;
	}
}
