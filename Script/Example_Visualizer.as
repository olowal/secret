
class UExample_ComponentDummyVisualizer : UActorComponent
{
    
}

class UExample_ComponentVisualizer : USecScriptVisualizer
{
    default VisualizedClass = UExample_ComponentDummyVisualizer::StaticClass();

    UFUNCTION(BlueprintOverride)
    void VisualizeComponent(const UActorComponent Component)
    {
        UExample_ComponentDummyVisualizer Comp = Cast<UExample_ComponentDummyVisualizer>(Component);
        if (!ensure((Comp != nullptr) && (Comp.GetOwner() != nullptr)))
            return;

        //Print("Hello there...");
        DrawWireSphere(Comp.Owner.ActorLocation, 120.0f, FLinearColor::Green);
        //VisualizeShape(Comp, Comp.ActionShape, Comp.ActionShapeTransform, FLinearColor(0.2f, 0.5f, 0.2f), 1.f);
    }   
}

class AExample_Visualizer : AActor
{
    UPROPERTY(DefaultComponent, RootComponent)
    USceneComponent RootComp;

    UPROPERTY(DefaultComponent)
    UExample_ComponentDummyVisualizer DummyComp;
}
