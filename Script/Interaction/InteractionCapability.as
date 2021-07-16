import Interaction.InteractionComponent;

class UInteractionCapability : USecCapability
{
    USecTriggerUserComponent TriggerUser;
    ASecCharacter Character;

    TArray<USecTriggerComponent> ActionTriggers;    // Trigger that we can interact with by pressing a button.
    TArray<USecTriggerComponent> VisibleTriggers;   // Triggers that may indicate that we are close enough to an object that is interactable

    USecTriggerComponent LastFocus;
    USecTriggerComponent LastAction;

    UFUNCTION(BlueprintOverride)
    void Setup()
    {
        Character = Cast<ASecCharacter>(Owner);
        TriggerUser = USecTriggerUserComponent::Get(Owner);
    }

    UFUNCTION(BlueprintOverride)
    ECapabilityActivation ShouldActivate() const
    {
        if(!IsActionActivated(GameplayAction::Interact))
            return ECapabilityActivation::DontActivate;
        if(ActionTriggers.Num() == 0)
            return ECapabilityActivation::DontActivate;
        return ECapabilityActivation::Network;
    }

    UFUNCTION(BlueprintOverride)
    void PreActivation(FActivationSyncParams& SyncParams)
    {
        SyncParams.AddObject(n"Trigger", ActionTriggers[0]);
    }

    UFUNCTION(BlueprintOverride)
    void OnActivate(FCapabilityActivationParams ActivationParams)
    {
        Print("Activated Interaction", 2.0f);
        USecTriggerComponent Trigger = Cast<USecTriggerComponent>(ActivationParams.GetObject(n"Trigger"));

		//TSubclassOf<USecAttribute> Attr;
		//FSecAttributeModifierContainer AttrMod = FSecAttributeModifierContainer(1.0f, Attr, ESecAttributeModifierType::Mul);
    }

    UFUNCTION(BlueprintOverride)
    ECapabilityDeactivation ShouldDeactivate() const
    {
        return ECapabilityDeactivation::Local;
    }

    UFUNCTION(BlueprintOverride)
    void PreTick(float DeltaTime)
    {
        if(!IsLocallyControlled())
            return;

        TriggerUser.GetVisibleTriggers(VisibleTriggers);

        USecTriggerComponent NewFocusComp = nullptr;

        if(VisibleTriggers.Num() > 0)
        {
            NewFocusComp = VisibleTriggers[0];
        }

        if(VisibleTriggers.Contains(LastFocus))
            NewFocusComp = LastFocus;

        if(NewFocusComp != nullptr && LastFocus == nullptr)
        {
            UInteractionComponent InteractionComp = Cast<UInteractionComponent>(NewFocusComp);
            InteractionComp.ShowInteractionWidget(Character);
            InteractionComp.OnInteractionFocusEnter();
        }
        else if(NewFocusComp == nullptr && LastFocus != nullptr)
        {
            UInteractionComponent InteractionComp = Cast<UInteractionComponent>(LastFocus);
            InteractionComp.OnInteractionFocusExit();
            InteractionComp.HideInteractionWidget();
        }

        TriggerUser.GetAvailableTriggers(ActionTriggers);
        USecTriggerComponent NewActionComp = nullptr;

        if(ActionTriggers.Contains(NewFocusComp))
            NewActionComp = NewFocusComp;

        if(NewActionComp != nullptr && LastAction == nullptr)
        {
            UInteractionComponent InteractionComp = Cast<UInteractionComponent>(NewActionComp);
            InteractionComp.OnInteractionActionEnter();
        }
        else if(NewActionComp == nullptr && LastAction != nullptr)
        {
            UInteractionComponent InteractionComp = Cast<UInteractionComponent>(LastAction);
            InteractionComp.OnInteractionActionExit();
        }

        LastFocus = NewFocusComp;
        LastAction = NewActionComp;
    }
}
