import Components.InteractableComponent;

class UCharacterInteractAction : USecCapability
{
    UInteractorComponent Interactor;

    bool bWasInRange = false;

    UFUNCTION(BlueprintOverride)
    void Setup()
    {
        Interactor = UInteractorComponent::GetOrCreate(Owner);
    }

    UFUNCTION(BlueprintOverride)
    ECapabilityActivation ShouldActivate() const
    {
        if(Interactor == nullptr)
        {
            return ECapabilityActivation::DontActivate;
        }

        if(!Interactor.HasValidInteractable())
        {
            return ECapabilityActivation::DontActivate;
        }

        return ECapabilityActivation::Local;
    }

    UFUNCTION(BlueprintOverride)
    ECapabilityDeactivation ShouldDeactivate() const
    {
        if(!Interactor.HasValidInteractable())
        {
            return ECapabilityDeactivation::Local;
        }

        return ECapabilityDeactivation::DontDeactivate;
    }

    UFUNCTION(BlueprintOverride)
    void OnActivate(FCapabilityActivationParams ActivationParams)
    {
        bWasInRange = false;
    }

    UFUNCTION(BlueprintOverride)
    void TickActive(float DeltaTime)
    {
        const bool bIsInRange = Interactor.Interactable.IsInRange(Owner.ActorLocation);

        if(bIsInRange && !bWasInRange)
        {
            Interactor.Interactable.HandleInteractionInRange();
        }
        else if(!bIsInRange && bWasInRange)
        {
            Interactor.Interactable.HandleInteractionOutOfRange();
        }

        if(IsActionActivated(GameplayAction::Interact))
        {
            Interactor.Interactable.HandleInteractionSuccess();
        }

        bWasInRange = bIsInRange;
    }
}
