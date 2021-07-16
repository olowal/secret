
class UInteractionWidget : USecUserWidget
{
    UFUNCTION(BlueprintEvent, meta = (DisplayName = "On Interaction Focus Enter"))
    void BP_OnInteractionFocusEnter() {}

    UFUNCTION(BlueprintEvent, meta = (DisplayName = "On Interaction Focus Exit"))
    void BP_OnInteractionFocusExit() {}

    UFUNCTION(BlueprintEvent, meta = (DisplayName = "On Interaction Action Enter"))
    void BP_OnInteractionActionEnter() {}

    UFUNCTION(BlueprintEvent, meta = (DisplayName = "On Interaction Action Exit"))
    void BP_OnInteractionActionExit() {}
}
