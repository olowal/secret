#pragma once

#include "Engine/DataAsset.h"
#include "ObjectiveSettings.generated.h"

class UObjectiveWidget;
class UObjectiveBase;

UENUM(BlueprintType)
enum class EObjectiveStateBehavior : uint8
{
	EOTCB_Complete UMETA(DisplayName = "Complete"),
	EOTCB_Fail UMETA(DisplayName = "Fail"),
	EOTCB_Nothing UMETA(DisplayName = "Nothing"),
};

/*
An objective counter is used when you want to create objectives like;
Pickup 6 food boxes 0/5
CounterDescription will then be the text "Pickup 6 food boxes" and Start will be 0 and Target is set to 5.
The ObjectiveWidget needs to be setup to display this properly.
*/
USTRUCT(BlueprintType)
struct FObjectiveSettingsCounterConfig
{
	GENERATED_BODY()
public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Counter)
    int32 Start = 0;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Counter)
    int32 Target = 1;

    // Define what will happen with this objective when it has reached the target counter.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Timer)
    EObjectiveStateBehavior OnTarget = EObjectiveStateBehavior::EOTCB_Complete;
};

USTRUCT(BlueprintType)
struct FObjectiveSettingsTimerConfig
{
    GENERATED_BODY()
public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Timer)
    float Start = 10.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Timer)
    float Target = 0.0f;

    // Define what will happen with this objective when it has reached the target time.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Timer)
    EObjectiveStateBehavior OnTarget = EObjectiveStateBehavior::EOTCB_Complete;
};

UCLASS(BlueprintType)
class SECRET_API UObjectiveSettings : public UDataAsset
{
	GENERATED_BODY()
public:

    UPROPERTY(EditDefaultsOnly, Category = Logic)
    bool bUseCounter = false;

    UPROPERTY(EditDefaultsOnly, Category = Logic, meta = (EditCondition = bUseCounter))
    FObjectiveSettingsCounterConfig Counter;

    UPROPERTY(EditDefaultsOnly, Category = Logic)
    bool bUseTimer = false;

    UPROPERTY(EditDefaultsOnly, Category = Logic, meta = (EditCondition = bUseTimer))
    FObjectiveSettingsTimerConfig Timer;

    // Starting this objective will also start these children objectives.
    UPROPERTY(EditDefaultsOnly, Category = Children)
    TArray<UObjectiveSettings*> ChildrenObjectives;

    // Optional: Used for UI, can be a header or general description of this objective.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = UI)
    FText ObjectiveDescription;

    // Optional: Override the default widget class set by the objective system actor with a specific one only for this objective.
    UPROPERTY(EditDefaultsOnly, Category = UI)
    TSubclassOf<UObjectiveWidget> OverrideDefaultWidgetClass;

    // Optional: Change the default objective sub object class for this specific objective. If you don't know what this is, don't touch it.
    UPROPERTY(EditDefaultsOnly, Category = "Advanced|Default")
    TSubclassOf<UObjectiveBase> DefaultObjectiveClass;
};
