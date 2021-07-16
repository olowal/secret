#pragma once

#include "Components/ActorComponent.h"
#include "SecTriggerUserComponent.generated.h"

class USecTriggerComponent;

UCLASS(BlueprintType, Blueprintable, ClassGroup = Trigger, meta = (BlueprintSpawnableComponent))
class SECRET_API USecTriggerUserComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	USecTriggerUserComponent(const FObjectInitializer& ObjectInitializer);

	// Add a new trigger for consideration into the user component
	void AddAvailableTriggerComponent(USecTriggerComponent* TriggerComponent);

	// Remove a trigger, it will no longer be considered for usage
	void RemoveAvailableTriggerComponent(USecTriggerComponent* TriggerComponent);

	// Add a new trigger for consideration into the user component
	void AddVisibleTriggerComponent(USecTriggerComponent* TriggerComponent);

	// Remove a trigger, it will no longer be considered for usage
	void RemoveVisibleTriggerComponent(USecTriggerComponent* TriggerComponent);

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Activate any triggers of the specified activation type that are available
	UFUNCTION(ScriptCallable)
	USecTriggerComponent* ActivateAnyAvailableTrigger();

	// Get a list of triggers we are close to of the specific activation type
	UFUNCTION(ScriptCallable)
	bool GetAvailableTriggers(TArray<USecTriggerComponent*>& OutTriggers);

	// Get a list of triggers we are close to
	UFUNCTION(ScriptCallable)
	bool GetAllAvailableTriggers(TArray<USecTriggerComponent*>& OutTriggers);

	// Get a list of triggers that are visible to us right now
	UFUNCTION(ScriptCallable)
	bool GetVisibleTriggers(TArray<USecTriggerComponent*>& OutTriggers);

	// Get a list of triggers that are visible to us right now
	UFUNCTION(ScriptCallable)
	bool GetAllVisibleTriggers(TArray<USecTriggerComponent*>& OutTriggers);

	// Check whether a trigger is currently available for activation
	UFUNCTION(ScriptCallable)
	bool IsTriggerAvailable(USecTriggerComponent* TriggerComponent);

	// Limit the trigger user to only use triggers with specific activation settings tags
	UFUNCTION(ScriptCallable)
	void SetTriggerRequiredTag(FName Tag);

	// Select a specific trigger to activate out of a list of available ones
	UFUNCTION(ScriptCallable)
	USecTriggerComponent* GetFocusedTrigger(const TArray<USecTriggerComponent*>& Triggers);

	UFUNCTION(BlueprintImplementableEvent, Meta = (ScriptName = "SelectFocusedTrigger"))
	USecTriggerComponent* BP_SelectFocusedTrigger(const TArray<USecTriggerComponent*>& Triggers) const;

protected:
	// Override this function to make general rules to all triggers
	UFUNCTION(ScriptCallable, BlueprintPure, BlueprintNativeEvent, Category = "SelectFocusedTrigger", meta = (ScriptName = "CanActivateTrigger"))
	bool BP_CanActivateTrigger(USecTriggerComponent* Trigger, bool bForVisuals) const;
	virtual bool BP_CanActivateTrigger_Implementation(USecTriggerComponent* Trigger, bool bForVisuals) const { return true; }

private:
	// Determine which triggers are available for us to use right now.
	void UpdateAvailableTriggers();

	// Determine which triggers are visible for us to use right now.
	void UpdateVisibleTriggers();

	bool CanActivateTrigger(USecTriggerComponent* Trigger, bool bForVisuals);

	/* Triggers that we are currently considering for usage. */
	UPROPERTY()
	TArray<USecTriggerComponent*> AvailableTriggers;

	/* Triggers that are currently visible to us. */
	UPROPERTY()
	TArray<USecTriggerComponent*> VisibleTriggers;

	/* Current required activation tag for triggers to be used. */
	FName RequiredActivationTag;
};
