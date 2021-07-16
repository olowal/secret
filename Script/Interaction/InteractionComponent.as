import Interaction.InteractionWidget;
event void FOnInteractionComponentActivated(UInteractionComponent Component, ASecCharacter Character);

UCLASS(HideCategories = "Activation Rendering Cooking Physics LOD AssetUserData Collision")
class UInteractionComponent : USecTriggerComponent
{
    UPROPERTY()
	TSubclassOf<UInteractionWidget> WidgetClass = Asset("/Game/Blueprints/UI/Interaction/WBP_Interaction.WBP_Interaction_C");
	private UInteractionWidget WidgetInstance;

	/* Event that is broadcast when the trigger is activated. */
    UPROPERTY(meta = (BPCannotCallEvent))
    FOnInteractionComponentActivated OnActivated;

	/* Whether to disable the interaction by default when it enters play. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Activate", meta = (InlineEditConditionToggle))
	bool bStartDisabled = false;

	/* Disable reason to disable with if the interaction enters play disabled. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Activate", meta = (EditCondition = "bStartDisabled"))
	FName StartDisabledReason = n"StartDisabled";

	/* Must be in this volume to consider this interaction for usage. */
	UPROPERTY(BlueprintReadOnly, EditInstanceOnly, Category = "ActionArea")
	AVolume ActionVolume = nullptr;

	/* Automatically create an action shape of this type. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "ActionArea", meta = (ShowOnlyInnerProperties))
	FSecShapeSettings ActionShape;
    default ActionShape.Type = ESecShapeType::Box;
	default ActionShape.BoxExtends = FVector(100.f, 100.f, 100.f);
	default ActionShape.SphereRadius = 100.f;

	/* The automatically created action shape should have this as its transform. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "ActionArea", meta = (MakeEditWidget))
	FTransform ActionShapeTransform = FTransform(FVector(0.f, 0.f, 100.f));

	/* Must be in this volume to consider this interaction for usage. */
	UPROPERTY(BlueprintReadOnly, EditInstanceOnly, Category = "FocusArea")
	AVolume FocusVolume = nullptr;

	/* Automatically create an action shape of this type. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FocusArea", meta = (ShowOnlyInnerProperties))
	FSecShapeSettings FocusShape;
    default FocusShape.Type = ESecShapeType::Sphere;
	default FocusShape.BoxExtends = FVector(100.f, 100.f, 100.f);
	default FocusShape.SphereRadius = 100.f;
	 
	/* The automatically created action shape should have this as its transform. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FocusArea", meta = (MakeEditWidget))
	FTransform FocusShapeTransform;
	default FocusShapeTransform.SetScale3D(FVector(7.f, 7.f, 7.f));

    UFUNCTION(BlueprintOverride)
    void BeginPlay()
    {
        // Set the configured action areas
        if (ActionVolume != nullptr)
            AddActionVolume(ActionVolume);
        //if (!ActionShape.IsZeroSize() && ActionShapeTransform.GetScale3D().GetMin() >= SMALL_NUMBER)
            AddActionShape(ActionShape, ActionShapeTransform);

        // Set the configured action areas
        if (FocusVolume != nullptr)
            AddFocusVolume(FocusVolume);
        //if (!FocusShape.IsZeroSize() && FocusShapeTransform.GetScale3D().GetMin() >= SMALL_NUMBER)
            AddFocusShape(FocusShape, FocusShapeTransform);

		if (bStartDisabled)
			Disable(StartDisabledReason);

        // Bind the Activation delegate
        FSecTriggerActivationDelegate Delegate;
        Delegate.BindUFunction(this, n"OnTriggerComponentActivated");
        AddActivationDelegate(Delegate);
    }

    UFUNCTION()
    void OnTriggerComponentActivated(USecTriggerComponent TriggerComponent, ASecCharacter Character)
    {
        OnActivated.Broadcast(this, Character);
    }

	void ShowInteractionWidget(ASecCharacter InteractionCharacter)
	{
		if(WidgetInstance == nullptr)
			WidgetInstance = Cast<UInteractionWidget>(SecUI::AttachWidgetToActor(WidgetClass, Owner, InteractionCharacter, EWidgetSpace::Screen));

		WidgetInstance.SetVisibility(ESlateVisibility::Visible);
	}

	void HideInteractionWidget()
	{
		if(WidgetInstance == nullptr)
			return;

		WidgetInstance.SetVisibility(ESlateVisibility::Collapsed);
	}

	void OnInteractionFocusEnter()
	{
		if(WidgetInstance == nullptr)
			return;

		WidgetInstance.BP_OnInteractionFocusEnter();
	}

	void OnInteractionFocusExit()
	{
		if(WidgetInstance == nullptr)
			return;

		WidgetInstance.BP_OnInteractionFocusExit();
	}

	void OnInteractionActionEnter()
	{
		if(WidgetInstance == nullptr)
			return;

		WidgetInstance.BP_OnInteractionActionEnter();
	}

	void OnInteractionActionExit()
	{
		if(WidgetInstance == nullptr)
			return;

		WidgetInstance.BP_OnInteractionActionExit();
	}
}
