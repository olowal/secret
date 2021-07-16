
#if EDITOR
class UExample_SecVisualizer : USecScriptVisualizer
{
    UFUNCTION(BlueprintOverride)
    void VisualizeComponent(const UActorComponent Comp)
    {
        PrintToScreen("VISUALIZE COMPONENT!");
    }

    default VisualizedClass = UExample_SecComponentVisualizerDummy::StaticClass();

}

class UExample_SecVisualizer_Part2 : USecScriptVisualizer
{
    UFUNCTION(BlueprintOverride)
    void VisualizeComponent(const UActorComponent Comp)
    {
        //FVector Extents(1000, 500, 2000);
        //DrawWireBox(Comp.Owner.ActorLocation, Extents, Comp.Owner.ActorQuat, FLinearColor::Blue, 10);
        //DrawWireSphere(Comp.Owner.ActorLocation, 800, FLinearColor::Red, 20, 64);
        DrawWireCapsule(Comp.Owner.ActorLocation, Comp.Owner.ActorRotation, FLinearColor::Yellow, 900, 1500, 16);
    }

    default VisualizedClass = UExample_SecComponentVisualizerDummy_Part2::StaticClass();

}

#endif // EDITOR

class UExample_SecComponentVisualizerDummy : UActorComponent
{
    default bIsEditorOnly = true;
}

class UExample_SecComponentVisualizerDummy_Part2 : UActorComponent
{
    default bIsEditorOnly = true;
}

class AExample_SecVisualizerActor : AActor
{
    UPROPERTY(DefaultComponent, RootComponent)
    USceneComponent Root;

    UPROPERTY(DefaultComponent)
    UExample_SecComponentVisualizerDummy_Part2 DummyVisualizer;

    UPROPERTY(DefaultComponent)
    UExample_SecComponentVisualizerDummy DummyVisualizer_Part2;
}
