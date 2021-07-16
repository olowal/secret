
class UBlockFaceDirectionCapability : USecCapability
{

    default Tags.Add(n"BlockFaceDirection");


    UFUNCTION(BlueprintOverride)
    ECapabilityActivation ShouldActivate() const
    {
        if(!Attribute.GetActionAttribute(GameplayAction::Block))
        {
            return ECapabilityActivation::DontActivate;
        }

        return ECapabilityActivation::Local;
    }

    UFUNCTION(BlueprintOverride)
    ECapabilityDeactivation ShouldDeactivate() const
    {
        if(!Attribute.GetActionAttribute(GameplayAction::Block))
        {
            return ECapabilityDeactivation::Local;
        }

        return ECapabilityDeactivation::DontDeactivate;
    }

    UFUNCTION(BlueprintOverride)
    void OnActivate(FCapabilityActivationParams ActivationParams)
    {
        BlockCapabilities(n"CharacterFaceDirection", this);
    }

    UFUNCTION(BlueprintOverride)
    void OnDeactivate(FCapabilityDeactivationParams& DeactivationParams)
    {
        UnblockCapabilities(n"CharacterFaceDirection", this);
    }
}
