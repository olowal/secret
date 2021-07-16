import AttributeModifiers.CharacterSpeedAttributeModifier;
import Components.SprintComponent;

class UCharacterSprintAction : USecCapability
{
    USecCharacterMovementComponent MoveComp;
	USprintComponent SprintComp;

    UFUNCTION(BlueprintOverride)
    void Setup()
    {
        MoveComp = USecCharacterMovementComponent::Get(Owner);
		SprintComp = USprintComponent::Get(Owner);
    }

    UFUNCTION(BlueprintOverride)
    ECapabilityActivation ShouldActivate() const
    {
        if(!MoveComp.IsMovingOnGround())
        {
            return ECapabilityActivation::DontActivate;
        }

        if(!Attribute.GetActionAttribute(GameplayAction::CharacterSprint))
        {
            return ECapabilityActivation::DontActivate;
        }

        if(MoveComp.Velocity.Size2D() <= 0.1f)
        {
            return ECapabilityActivation::DontActivate;
        }

        return ECapabilityActivation::Network;
    }

    UFUNCTION(BlueprintOverride)
    ECapabilityDeactivation ShouldDeactivate() const
    {
        if(!MoveComp.IsMovingOnGround())
        {
            return ECapabilityDeactivation::Network;
        }

        if(!Attribute.GetActionAttribute(GameplayAction::CharacterSprint))
        {
            return ECapabilityDeactivation::Network;
        }

        if(MoveComp.Velocity.Size2D() <= 0.1f)
        {
            return ECapabilityDeactivation::Network;
        }

        return ECapabilityDeactivation::DontDeactivate;
    }

    UFUNCTION(BlueprintOverride)
    void OnActivate(FCapabilityActivationParams ActivationParams)
    {
        SprintComp.bIsSprinting = true;
        BlockCapabilities(n"CharacterBlock", this);
        BlockCapabilities(n"BlockFaceDirection", this);
        
        //if(MoveComp.SprintModifierSheet != nullptr)
        {
			//Attribute.ApplyAttributeModifierSheet(MoveComp.SprintModifierSheet, this, ESecScriptPriority::Script);
			TSubclassOf<USecAttribute> Attr = USecCharacterMovementSpeedMultiplierAttribute::StaticClass();
			Attribute.ApplyModifier(FSecAttributeModifierContainer(1.5f, Attr, ESecAttributeModifierType::Mul), this, ESecScriptPriority::High);
        }
    }

    UFUNCTION(BlueprintOverride)
    void OnDeactivate(FCapabilityDeactivationParams& DeactivationParams)
    {
        SprintComp.bIsSprinting = false;
        UnblockCapabilities(n"CharacterBlock", this);
        UnblockCapabilities(n"BlockFaceDirection", this);
        //Attribute.RemoveAttributeModifierSheetByInstigator(this);
		Attribute.RemoveModifierSheetByInstigator(this);
	}
}
