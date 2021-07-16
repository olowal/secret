#pragma once

#include "UObject/Object.h"
#include "Tickable.h"
#include "ObjectiveStatics.h"
#include "UI/ObjectiveUIStatics.h"
#include "ObjectiveBase.generated.h"

class UWorld;
class AObjectiveSystemActor;
class UObjectiveWidget;
class UObjectiveSettings;

UENUM(BlueprintType)
enum class EObjectiveState : uint8
{
    EOS_Completed UMETA(DisplayName = "Completed"),
    EOS_Failed UMETA(DisplayName = "Failed"),
    EOS_InProgress UMETA(DisplayName = "InProgress"),
    EOS_None UMETA(DisplayName = "None")
};

USTRUCT()
struct FObjectiveReplicatedCounterData
{
    GENERATED_BODY()
public:
    UPROPERTY()
    int32 Current = 0;

    UPROPERTY()
    int32 Target = 1;
};

USTRUCT()
struct FObjectiveReplicatedTimerData
{
    GENERATED_BODY()
public:
    UPROPERTY(NotReplicated) // Replication updated in looped multicast.
    float Current = 10.0f;

    UPROPERTY()
    float Target = 0.0f;

    UPROPERTY()
    bool bPaused = false;
    UPROPERTY()
    bool bDone = false;
};

USTRUCT()
struct FUpdateObjectiveTimerReplication
{
    GENERATED_BODY()
public:
    UPROPERTY()
    float Current = 0.0f;
};

UCLASS(BlueprintType, Blueprintable)
class SECRET_API UObjectiveBase : public UObject, public FTickableGameObject
{
	GENERATED_BODY()
public:

    virtual void InitObjective();
    virtual void OnDestroyObjective();

	virtual void Tick(float DeltaTime) override;

	virtual bool IsTickable() const override;
    virtual bool IsTickableWhenPaused() const override { return bTickTimerWhenPaused; }

	virtual int32 GetFunctionCallspace(UFunction* Function, FFrame* Stack) override;
	virtual bool CallRemoteFunction(UFunction* Function, void* Parameters, FOutParmRec* OutParms, FFrame* Stack) override;
	virtual bool IsSupportedForNetworking() const override { return true; }

    virtual TStatId GetStatId() const override;

    virtual UWorld* GetWorld() const override;

    void Start();
    void Complete();
    void Fail();
    void Progress(int32 Value);

    UPROPERTY(Transient)
    TArray<UObjectiveBase*> Children;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = Objective)
    UObjectiveBase* ParentObjective;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = Objective)
    UObjectiveSettings* Settings;

    UPROPERTY(Transient)
    AObjectiveSystemActor* ObjectiveSystem;

    bool HasAuthority() const;

    UObjectiveWidget* SetupWidget();

    void SetupUI();

    FObjectiveUIInfo CreateUIInfo() const;

    bool IsTimerPaused() const { return ReplicatedTimer.bPaused; }
    bool IsTimerDone() const { return ReplicatedTimer.bDone; }

protected:

    UFUNCTION()
    void ReplicateTimers();

    UFUNCTION(NetMulticast, Unreliable)
    void Multicast_UpdateTimers(float NewTime);

    UFUNCTION()
    virtual void OnRep_Counter();

    UPROPERTY(ReplicatedUsing = OnRep_Counter)
    FObjectiveReplicatedCounterData ReplicatedCounter;

    UPROPERTY(Replicated)
    FObjectiveReplicatedTimerData ReplicatedTimer;

    FTimerHandle TimerReplicationHandle;

    UPROPERTY(Transient)
    UObjectiveWidget* WidgetInstance;

    UFUNCTION()
    virtual void OnRep_ObjectiveState(EObjectiveState OldObjectiveState);

    UPROPERTY(ReplicatedUsing = OnRep_ObjectiveState)
    EObjectiveState ObjectiveState = EObjectiveState::EOS_None;

    UPROPERTY(EditAnywhere, Category = Tick)
    bool bTickTimerWhenPaused = false;

    bool bInitialized = false;
};
