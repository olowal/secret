#pragma once

#include "Components/SceneComponent.h"
#include "Shape/SecShapeStatics.h"
#include "Trigger/SecTriggerCondition.h"
#include "SecTriggerComponent.generated.h"

class AVolume;
class ASecCharacter;

DECLARE_DYNAMIC_DELEGATE_TwoParams(FSecTriggerActivationDelegate, USecTriggerComponent*, Trigger, ASecCharacter*, Player);
DECLARE_DELEGATE_TwoParams(FSecTriggerActivationCodeDelegate, USecTriggerComponent*, ASecCharacter*);

USTRUCT()
struct FSecTriggerDisableReasonContainer
{
	GENERATED_BODY()
public:
	FSecTriggerDisableReasonContainer() {}
	FSecTriggerDisableReasonContainer(TWeakObjectPtr<ASecCharacter> InCharacter, FName InReason) :
		Character(InCharacter) 
	{
		Reasons.Add(InReason);
	}

	UPROPERTY()
	TArray<FName> Reasons;

	UPROPERTY()
	TWeakObjectPtr<ASecCharacter> Character;

	bool operator == (const FSecTriggerDisableReasonContainer& LHS) const 
	{ 
		if (this->Character != LHS.Character)
			return false;

		return LHS.Reasons == this->Reasons;
	}
};

USTRUCT()
struct FSecTriggerConditionContainer
{
	GENERATED_BODY()
public:
	TMap<FName, TArray<FSecTriggerCondition>> Conditions;

	UPROPERTY()
	TWeakObjectPtr<ASecCharacter> Character;
};

UCLASS(BlueprintType, ClassGroup = "Trigger", meta = (BlueprintSpawnableComponent))
class USecTriggerComponent : public USceneComponent
{
	GENERATED_BODY()
public:
	/* Add a volume as an action area. */
	UFUNCTION(BlueprintCallable, Category = "Trigger | Action Area")
	void AddActionVolume(AVolume* Volume);

	/* Add a specific shape as an action area. */
	UFUNCTION(BlueprintCallable, Category = "Trigger | Action Area")
	void AddActionShape(FSecShapeSettings ShapeSettings, const FTransform& ShapeTransform);

	/* Add an arbitrary primitive component as an action area. */
	UFUNCTION(BlueprintCallable, Category = "Trigger | Action Area")
	void AddActionPrimitive(UPrimitiveComponent* Primitive);

	/* Focus areas determine when the trigger becomes visible in the UI. */

	/* Add a volume as a focus area. */
	UFUNCTION(BlueprintCallable, Category = "Trigger | Focus Area")
	void AddFocusVolume(AVolume* Volume);

	/* Add a specific shape as a focus area. */
	UFUNCTION(BlueprintCallable, Category = "Trigger | Focus Area")
	void AddFocusShape(FSecShapeSettings ShapeSettings, const FTransform& ShapeTransform);

	/* Add an arbitrary primitive component as a focus area. */
	UFUNCTION(BlueprintCallable, Category = "Trigger | Focus Area")
	void AddFocusPrimitive(UPrimitiveComponent* Primitive);

	/* Set this trigger as disabled for a specific reason. */
	UFUNCTION(BlueprintCallable, Category = "Trigger | Disabling")
	void Disable(FName Reason);

	/* Remove a disable reason that was previously added to this trigger. */
	UFUNCTION(BlueprintCallable, Category = "Trigger | Disabling")
	void Enable(FName Reason);

	/* Set this trigger as disabled for a specific reason. */
	UFUNCTION(BlueprintCallable, Category = "Trigger | Disabling")
	void DisableForCharacter(ASecCharacter* Character, FName Reason);

	/* Remove a disable reason that was previously added to this trigger. */
	UFUNCTION(BlueprintCallable, Category = "Trigger | Disabling")
	void EnableForCharacter(ASecCharacter* Character, FName Reason);

	/* Check whether the trigger is currently disabled for a specific reason. */
	UFUNCTION(BlueprintPure, Category = "Trigger | Disabling")
	bool IsDisabledForCharacter(FName Reason, ASecCharacter* ForCharacter);

	/* Check whether the trigger is currently disabled for a specific reason. */
	UFUNCTION(BlueprintPure, Category = "Trigger | Disabling")
	bool IsDisabled(FName Reason);

	/* Check whether the trigger is currently disabled by any reason. */
	UFUNCTION(BlueprintPure, Category = "Trigger | Disabling")
	bool IsDisabledForAnyReason();

	/* Check whether the trigger is currently disabled by any reason. */
	UFUNCTION(BlueprintPure, Category = "Trigger | Disabling")
	bool IsDisabledForCharacterForAnyReason(ASecCharacter* ForCharacter);

	/* Add a specific condition to check when determining whether this trigger can be used. */
	UFUNCTION(BlueprintCallable, Category = "Trigger | Conditions")
	FSecTriggerConditionHandle AddTriggerCondition(FName Reason, FSecTriggerCondition Condition);

	/* Add a specific condition to check when determining whether this trigger can be used. */
	UFUNCTION(BlueprintCallable, Category = "Trigger | Conditions")
	FSecTriggerConditionHandle AddCharacterCondition(ASecCharacter* ForCharacter, FName Reason, FSecTriggerCondition Condition);

	// Re-check all conditions and update the disable reasons state
	void UpdateConditions();
	void UpdateConditionsForCharacter(FName Reason, ASecCharacter* Character);

	/* Bind a delegate to the activation of this trigger. */
	UFUNCTION(BlueprintCallable, Category = "Trigger | Activation")
	void AddActivationDelegate(FSecTriggerActivationDelegate Delegate);

	/* Bind a code delegate to the activation of this trigger. */
	void AddActivationDelegate(FSecTriggerActivationCodeDelegate Delegate);

	/* Move to the trigger using its default movement settings and attempt to activate. */
	UFUNCTION(BlueprintCallable, Category = "Trigger | Activation")
	void StartActivating(ASecCharacter* Player);

private:
	void BindToActionPrimitive(UPrimitiveComponent* Component);

	UFUNCTION()
	void OnActionPrimitiveBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
	void OnActionPrimitiveEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void EnterActionArea(UPrimitiveComponent* ActionArea, AActor* EnteredActor);
	void LeaveActionArea(UPrimitiveComponent* ActionArea, AActor* LeftActor);

	void BindToFocusPrimitive(UPrimitiveComponent* Component);

	UFUNCTION()
	void OnFocusPrimitiveBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
	void OnFocusPrimitiveEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void EnterFocusArea(UPrimitiveComponent* ActionArea, AActor* EnteredActor);
	void LeaveFocusArea(UPrimitiveComponent* FocusArea, AActor* LeftActor);

	/* Action shapes we are currently tracking actors inside of. */
	TMap<TWeakObjectPtr<AActor>, TArray<TWeakObjectPtr<UPrimitiveComponent>>> ActionAreaOverlaps;

	/* Focus shapes we are currently tracking actors inside of. */
	TMap<TWeakObjectPtr<AActor>, TArray<TWeakObjectPtr<UPrimitiveComponent>>> FocusAreaOverlaps;

	UPROPERTY(Transient)
	TArray<FSecTriggerDisableReasonContainer> CharacterDisableReasons;

	TArray<FName> DisableReasons;

	TArray<FSecTriggerConditionContainer> Conditions;

	/* Activation delegates that will be called when this gets activated. */
	UPROPERTY()
	TArray<FSecTriggerActivationDelegate> ActivationDelegates;

	/* Activation delegates that will be called when this gets activated. */
	TArray<FSecTriggerActivationCodeDelegate> ActivationCodeDelegates;
};
