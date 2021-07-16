class AExample_Actor : AActor
{
    UPROPERTY(DefaultComponent, RootComponent)
    USceneComponent Root;

	UPROPERTY(Replicated)
	float TickTest = 0;

	default bReplicates = true;

	UPrimitiveComponent PrimComp;

	UFUNCTION(BlueprintOverride)
	void BeginPlay()
	{
		if(HasAuthority())
			TickTest = 10;


		FSecShapeSettings Shape;
		Shape.Type = ESecShapeType::Box;
		Shape.BoxExtends = FVector(500, 500, 500);
		PrimComp = SecShape::CreateTriggerShape(this, Root, Shape, FTransform::Identity, n"BlockAll");
		PrimComp.SetHiddenInGame(false);
	}

	UFUNCTION(BlueprintOverride)
	void Tick(float DeltaTime)
	{
		PrintToScreen("TickTest " + TickTest);
	}
}
