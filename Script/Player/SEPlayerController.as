
class ASEPlayerController : ASecPlayerControllerBase
{
    default bAutoManageActiveCameraTarget = true;

    ASecThirdPersonCameraBase ThirdPersonCamera;

    UFUNCTION(BlueprintOverride)
    void LocalOnPossessedPawn()
    {
		bShowMouseCursor = true;
    }
}
