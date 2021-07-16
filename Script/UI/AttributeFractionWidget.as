
/*
Widget for calculating get receiving events from the use of two attribute values.
*/

class UAttributeFractionWidget : USecUserWidget
{
	UPROPERTY()
	TSubclassOf<USecAttribute> AttributeClass;
	USecAttribute AttributeInstance = nullptr;
	
	UPROPERTY()
	TSubclassOf<USecAttribute> AttributeMaximumClass;
	USecAttribute AttributeMaximumInstance = nullptr;

	private bool bAttributeInitialized = false;
	private bool bAttributeMaximumInitialized = false;

	UFUNCTION(BlueprintOverride)
	void Construct()
	{
		AActor ActorOwner = Cast<AActor>(ObjectOwner);

		USecAttributeComponent AttributeComp = USecAttributeComponent::Get(ActorOwner);

		if(!ensure(AttributeComp != nullptr))
			return;

		AttributeInstance = AttributeComp.GetAttributeByClass(AttributeClass);
		AttributeMaximumInstance = AttributeComp.GetAttributeByClass(AttributeMaximumClass);

		if(AttributeInstance == nullptr || AttributeMaximumInstance == nullptr)
		{
			AttributeComp.OnAttributeInstanced.AddUFunction(this, n"Handle_OnAttributeInstanced");
		}

		if(AttributeInstance != nullptr)
		{
			SetupAttributeDelegates(AttributeInstance, AttributeComp);
		}

		if(AttributeMaximumInstance != nullptr)
		{
			SetupAttributeMaximumDelegates(AttributeMaximumInstance, AttributeComp);
		}
	}

	UFUNCTION()
	private void Handle_OnAttributeInstanced(USecAttribute Attribute, USecAttributeComponent AttributeComponent)
	{
		SetupAttributeDelegates(Attribute, AttributeComponent);
		SetupAttributeMaximumDelegates(Attribute, AttributeComponent);
	}

	private void SetupAttributeDelegates(USecAttribute Attribute, USecAttributeComponent AttributeComponent)
	{
		if(Attribute.Class != AttributeClass)
			return;

		if(bAttributeInitialized)
			return;

		bAttributeInitialized = true;
		AttributeInstance = Attribute;
		AttributeInstance.OnModified.AddUFunction(this, n"Handle_OnAttributeModified");
		
		FinishSetup(AttributeComponent);
	}

	private void SetupAttributeMaximumDelegates(USecAttribute Attribute, USecAttributeComponent AttributeComponent)
	{
		if(Attribute.Class != AttributeMaximumClass)
			return;

		if(bAttributeMaximumInitialized)
			return;
		
		bAttributeMaximumInitialized = true;
		AttributeMaximumInstance = Attribute;
		AttributeMaximumInstance.OnModified.AddUFunction(this, n"Handle_OnAttributeMaximumModified");

		FinishSetup(AttributeComponent);
	}

	UFUNCTION(BlueprintEvent, meta = (DisplayName = "On Setup"))
	void BP_OnSetup(float Attribute, float AttributeMaximum, float Fraction) {}

	UFUNCTION(BlueprintPure, meta = (DisplayName = "GetAttributeValue"))
	float GetAttributeValue() const property
	{
		if(AttributeInstance == nullptr)
			return 0;

		return AttributeInstance.AttributeValue;
	}

	UFUNCTION(BlueprintPure, meta = (DisplayName = "GetAttributeValueMaximum"))
	float GetAttributeMaximumValue() const property
	{
		if(AttributeMaximumInstance == nullptr)
			return 0;

		return AttributeMaximumInstance.AttributeValue;
	}

	// for both attributes
	private void FinishSetup(USecAttributeComponent AttributeComponent)
	{
		if(!bAttributeInitialized || !bAttributeMaximumInitialized)
			return;

		AttributeComponent.OnAttributeInstanced.UnbindObject(this);
		const float Fraction = CalculateFractionSafe(AttributeValue, AttributeMaximumValue);
		BP_OnSetup(AttributeValue, AttributeMaximumValue, Fraction);
	}

	UFUNCTION(BlueprintEvent, meta = (DisplayName = "On Attribute Modified"))
	void BP_OnAttributeModified(float NewValue, float OldValue, float AttributeMaximum, float Fraction) {}

	UFUNCTION(BlueprintEvent, meta = (DisplayName = "On Attribute Maximum Modified"))
	void BP_OnAttributeMaximumModified(float NewValue, float OldValue, float Attribute, float Fraction) {}

	UFUNCTION()
	private void Handle_OnAttributeModified(float NewValue, float OldValue)
	{
		const float AttributeMaximum = AttributeMaximumValue;
		const float Fraction = CalculateFractionSafe(NewValue, AttributeMaximum);
		BP_OnAttributeModified(NewValue, OldValue, AttributeMaximum, Fraction);
	}

	UFUNCTION()
	private void Handle_OnAttributeMaximumModified(float NewValue, float OldValue)
	{
		const float Attribute = AttributeValue;
		const float Fraction = CalculateFractionSafe(Attribute, NewValue);
		BP_OnAttributeMaximumModified(NewValue, OldValue, Attribute, Fraction);
	}

	private float CalculateFractionSafe(float A, float B) const
	{
		return FMath::IsNearlyZero(B) ? 0.0f : A / B;
	}
}
