import AttributeModifiers.BlockingAttributeModifier;
import Equipment.ShieldComponent;
import Components.TakeDamageComponent;
import Damage.DamageModifiers.ShieldBlockDamageModifier;
import AI.Sensing.SensingStatics;

class UBlockAttributeModifier : UAttributeModifier
{
    //default Modifiers.Add(SecAttribute::CreateModifierInfo(n"CharacterMovementSpeedMultiplier", EAttributeModifierType::Mul, 0.5f));
}

class UCharacterBlockCapability : USecCapability
{
	bool bToggle = true;

    UCharacterMovementComponent MoveComp;
	UTakeDamageComponent TakeDamage;
	UShieldComponent ShieldComp;

    default Tags.Add(n"CharacterBlock");

    UFUNCTION(BlueprintOverride)
    void Setup()
    {
        MoveComp = UCharacterMovementComponent::Get(Owner);
		ShieldComp = UShieldComponent::Get(Owner);
		TakeDamage = UTakeDamageComponent::Get(Owner);
    }

    UFUNCTION(BlueprintOverride)
    ECapabilityActivation ShouldActivate() const
    {
		if(!ShieldComp.bIsEquipped)
			return ECapabilityActivation::DontActivate;

		if(ShieldComp.bIsBlocking)
			return ECapabilityActivation::DontActivate;

        if(!MoveComp.IsMovingOnGround())
            return ECapabilityActivation::DontActivate;

        if(!bToggle && !IsAction(GameplayAction::Block))
            return ECapabilityActivation::DontActivate;

        if(bToggle && !IsActionActivated(GameplayAction::Block))	// Toggle capability instead of holding down the button (Also change ShouldDeactivate)
            return ECapabilityActivation::DontActivate;

        return ECapabilityActivation::Network;
    }

	UFUNCTION(BlueprintOverride)
	bool RemoteShouldActivate() const
	{
		return ShieldComp.ShieldSettings != nullptr;
	}

    UFUNCTION(BlueprintOverride)
    ECapabilityDeactivation ShouldDeactivate() const
    {
		if(!ShieldComp.bIsEquipped)
			return ECapabilityDeactivation::Network;

		if(!ShieldComp.bIsBlocking)
			return ECapabilityDeactivation::Network;

        if(!MoveComp.IsMovingOnGround())
            return ECapabilityDeactivation::Network;

        if(!bToggle && !IsAction(GameplayAction::Block))
            return ECapabilityDeactivation::Network;

        if(bToggle && IsActionActivated(GameplayAction::Block))
            return ECapabilityDeactivation::Network;
        
        return ECapabilityDeactivation::DontDeactivate;
    }

    UFUNCTION(BlueprintOverride)
    void OnActivate(FCapabilityActivationParams ActivationParams)
    {
		TakeDamage.AddDamageModifier(UShieldBlockDamageModifier::StaticClass(), this, ESecScriptPriority::High);
		ShieldComp.bIsBlocking = true;

		TSubclassOf<USecAttribute> Attr = USecCharacterRotationSpeedMultiplierAttribute::StaticClass();
		Attribute.ApplyModifier(FSecAttributeModifierContainer(0.5f, Attr, ESecAttributeModifierType::Mul), this, ESecScriptPriority::High);

		UCharacterMovementComponent TempMoveComp = UCharacterMovementComponent::Get(Owner);
	}

    UFUNCTION(BlueprintOverride)
    void OnDeactivate(FCapabilityDeactivationParams& DeactivationParams)
    {
		TakeDamage.RemoveDamageModifierByClass(UShieldBlockDamageModifier::StaticClass());
		Attribute.RemoveModifierByInstigator(this);
		ShieldComp.bIsBlocking = false;
    }

	UFUNCTION(BlueprintOverride)
	void PreTick(float DeltaTime)
	{
		//PrintToScreen("Num Crumbs: " + NumCrumbs);

		FVector Direction = Owner.ActorForwardVector;
		FVector LocationOrigin = Owner.ActorLocation;

		FVector LocationToTest = FVector(-10, -481, 80);
		System::DrawDebugSphere(LocationToTest, 10, 12, FLinearColor::Green);
		float Angle = 90.0f;
		float Distance = 600.0f;
		System::DrawDebugConeInDegrees(LocationOrigin + FVector::UpVector * 20.0f, Direction, Distance, Angle, 1.0f);

		bool bTest = SensingStatics::IsPointInsideCone2D(LocationOrigin, Direction, LocationToTest, Angle, Distance);
		PrintToScreen("bTest " + bTest);
	}
}
