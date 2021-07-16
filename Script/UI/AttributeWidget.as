
class UAttributeWidget : USecUserWidget
{
	UPROPERTY()
	TSubclassOf<USecAttribute> AttributeClass;
	private USecAttribute AttributeInstance;

	private bool bInitialized = false;

	UFUNCTION(BlueprintOverride)
	void Construct()
	{
		AActor ActorOwner = Cast<AActor>(ObjectOwner);

		USecAttributeComponent AttributeComp = USecAttributeComponent::Get(ActorOwner);

		if(!ensure(AttributeComp != nullptr))
			return;

		USecAttribute Attribute = AttributeComp.GetAttributeByClass(AttributeClass);

		if(Attribute == nullptr)
		{
			AttributeComp.OnAttributeInstanced.AddUFunction(this, n"Handle_OnAttributeInstanced");
			SetVisibility(ESlateVisibility::Collapsed);
		}
		else
		{
			SetupAttributeDelegates(Attribute, AttributeComp);
		}
	}

	UFUNCTION()
	private void Handle_OnAttributeInstanced(USecAttribute Attribute, USecAttributeComponent AttributeComponent)
	{
		SetupAttributeDelegates(Attribute, AttributeComponent);
	}

	private void SetupAttributeDelegates(USecAttribute Attribute, USecAttributeComponent AttributeComponent)
	{
		if(Attribute.Class != AttributeClass)
			return;

		if(bInitialized)
			return;

		bInitialized = true;
		AttributeInstance = Attribute;
		AttributeInstance.OnModified.AddUFunction(this, n"Handle_OnAttributeModified");
		BP_OnSetup(AttributeInstance.AttributeValue);
		AttributeComponent.OnAttributeInstanced.UnbindObject(this);
		SetVisibility(ESlateVisibility::Visible);
	}

	UFUNCTION(BlueprintPure, meta = (DisplayName = "GetAttributeValue"))
	float GetAttributeValue() const
	{
		if(AttributeInstance == nullptr)
			return 0.0f;

		return AttributeInstance.AttributeValue;
	}

	UFUNCTION(BlueprintEvent, meta = (DisplayName = "On Setup"))
	void BP_OnSetup(float Attribute) {}

	UFUNCTION(BlueprintEvent, meta = (DisplayName = "On Attribute Modified"))
	void BP_OnAttributeModified(float NewValue, float OldValue) {}

	UFUNCTION()
	private void Handle_OnAttributeModified(float NewValue, float OldValue)
	{
		BP_OnAttributeModified(NewValue, OldValue);
	}
}
