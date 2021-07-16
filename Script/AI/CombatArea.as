
class UCombatAreaVisualizerDummyComponent : UActorComponent 
{
	UPROPERTY(Category = Debug)
	FLinearColor VisualizerColor = FLinearColor::Green;
}

#if !RELEASE
class UCombatAreaVisualizerComponent : USecScriptVisualizer
{
    default VisualizedClass = UCombatAreaVisualizerDummyComponent::StaticClass();

    UFUNCTION(BlueprintOverride)
    void VisualizeComponent(const UActorComponent Component)
    {
        if (!ensure((Component != nullptr) && (Component.Owner != nullptr)))
            return;

		UCombatAreaVisualizerDummyComponent VisualizerComp = Cast<UCombatAreaVisualizerDummyComponent>(Component);
		ACombatArea CombatArea = Cast<ACombatArea>(Component.Owner);

		if(!ensure(CombatArea != nullptr))
			return;

        DrawWireSphere(CombatArea.ActorLocation, CombatArea.Radius, VisualizerComp.VisualizerColor);

    }   
}
#endif // !RELEASE

/*
	Constrain movement within a specific radius. AI moving using SecBTTask_MoveTo will be unable to move outside of the specified radius.
*/

class ACombatArea : ASecCombatArea
{
	UPROPERTY(DefaultComponent, ShowOnActor)
	UCombatAreaVisualizerDummyComponent VisualizerComponent;
	default VisualizerComponent.bIsEditorOnly = true;

	// Chase targets within this radius, should be larger than MovementRadius.
	UPROPERTY()
	float Radius = 1000;

	float GetRadiusSq() const property { return FMath::Square(Radius); }

	UFUNCTION(BlueprintOverride)
	bool ValidateLocation(FVector& LocationToValidate) const
	{
		const FVector SourceLocation = ActorLocation;
		const float DistanceFromOrigin = SourceLocation.DistSquared2D(LocationToValidate);

		if(DistanceFromOrigin > RadiusSq)
		{
			LocationToValidate = SourceLocation + ((LocationToValidate - SourceLocation).GetSafeNormal2D() * Radius);
			return true;
		}

		return false;
	}
}
