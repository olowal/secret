
class UInteractorComponent : UActorComponent
{
    void SetInteractable(UInteractableComponent NewInteractableComponent)
    {
        if(CurrentInteractable == nullptr)
        {
            CurrentInteractable = NewInteractableComponent;
        }
    }

    void ClearInteractable(UInteractableComponent Interactable)
    {
        if(Interactable == CurrentInteractable)
        {
            CurrentInteractable = nullptr;
        }
    }

    bool HasValidInteractable() const
    {
        return CurrentInteractable != nullptr;
    }

    UInteractableComponent GetInteractable() const property { return CurrentInteractable; }
    
    private UInteractableComponent CurrentInteractable;
}

struct FInteractionInfo
{
    UPROPERTY()
    UInteractableComponent Interactable;

    UPROPERTY()
    UInteractorComponent Interactor;
}

event void FInteractionDelegate(const FInteractionInfo& InteractionInfo);

class UInteractableComponent : USphereComponent
{
    UPROPERTY()
    FInteractionDelegate OnInteractionVisible;

    UPROPERTY()
    FInteractionDelegate OnInteractionNotVisible;

    UPROPERTY()
    FInteractionDelegate OnInteractionInRange;

    UPROPERTY()
    FInteractionDelegate OnInteractionOutOfRange;

    UPROPERTY()
    FInteractionDelegate OnInteractionSuccess;

    UPROPERTY()
    protected float ActionArea = 500.0f;

    UFUNCTION(BlueprintOverride)
    void BeginPlay()
    {
        OnComponentBeginOverlap.AddUFunction(this, n"HandleOverlapBegin");
        OnComponentEndOverlap.AddUFunction(this, n"HandleOverlapEnd");
    }

    bool IsInRange(const FVector& Location) const
    {
        return WorldLocation.DistSquared(Location) < FMath::Square(ActionArea);;
    }

    UFUNCTION()
    void HandleOverlapBegin(UPrimitiveComponent OverlappedComponent, AActor OtherActor, UPrimitiveComponent OtherComp, int OtherBodyIndex, bool bFromSweep, FHitResult& SweepResult)
    {
        UInteractorComponent Interactor = UInteractorComponent::Get(OtherActor);

        if(Interactor != nullptr)
        {
            Interactor.SetInteractable(this);
            HandleInteractionVisible();
        }
    }

    UFUNCTION()
    void HandleOverlapEnd(UPrimitiveComponent OverlappedComponent, AActor OtherActor, UPrimitiveComponent OtherComp, int OtherBodyIndex)
    {
        UInteractorComponent Interactor = UInteractorComponent::Get(OtherActor);

        if(Interactor != nullptr)
        {
            HandleInteractionNotVisible();
            Interactor.ClearInteractable(this);
        }
    }

    void HandleInteractionVisible()
    {
        FInteractionInfo InteractionInfo;
        OnInteractionVisible.Broadcast(InteractionInfo);
    }

    void HandleInteractionNotVisible()
    {
        FInteractionInfo InteractionInfo;
        OnInteractionNotVisible.Broadcast(InteractionInfo);
    }

    void HandleInteractionInRange()
    {
        FInteractionInfo InteractionInfo;
        OnInteractionInRange.Broadcast(InteractionInfo);
    }

    void HandleInteractionOutOfRange()
    {
        FInteractionInfo InteractionInfo;
        OnInteractionOutOfRange.Broadcast(InteractionInfo);
    }

    void HandleInteractionSuccess()
    {
        FInteractionInfo InteractionInfo;
        OnInteractionSuccess.Broadcast(InteractionInfo);
    }
}
