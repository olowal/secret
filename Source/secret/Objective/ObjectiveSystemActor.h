#pragma once

#include "GameFramework/Actor.h"
#include "ObjectiveStatics.h"
#include "ObjectiveSystemActor.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(ObjectiveSystemActorLog, Log, All);


class UObjectiveSettings;
class UObjectiveBase;
class UObjectiveHUDWidget;
class UObjectiveWidget;

USTRUCT()
struct FObjectiveDelegateContainer
{
	GENERATED_BODY()
public:
	FObjectiveDelegateContainer(){}
	FObjectiveDelegateContainer(UObjectiveSettings* InObjective)
	: Objective(InObjective){}

	TArray<FObjectiveCallbackSignature> Delegates;
	UPROPERTY(Transient)
	UObjectiveSettings* Objective;

	bool operator == (const FObjectiveDelegateContainer& LHS) const { return this->Objective == LHS.Objective; }
};

UCLASS()
class SECRET_API AObjectiveSystemActor : public AActor
{
	GENERATED_BODY()
public:
	AObjectiveSystemActor(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	UObjectiveBase* CreateObjective(UObjectiveSettings* ObjectiveSettings);
	void CreateObjectiveDelegate(UObjectiveSettings* ObjectiveSettings, EObjectiveCallbackType CallbackType, const FObjectiveCallbackSignature& Delegate);
	void BroadcastDelegates(UObjectiveSettings* ObjectiveSettings, EObjectiveCallbackType CallbackType);

	// Locate an objective and attempt to start it.
	void StartObjective(UObjectiveSettings* ObjectiveSettings);

	void CompleteObjective(UObjectiveSettings* ObjectiveSettings);
	void FailObjective(UObjectiveSettings* ObjectiveSettings);

	void ProgressObjective(UObjectiveSettings* ObjectiveSettings, int32 Value);

	// Trigger the HUD instance so we can add this to it's list.
	void AddToUI(UObjectiveBase* Objective);
	void SetUIHidden(bool bHidden);

	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

	UFUNCTION()
	void OnRep_ObjectiveList();

	UPROPERTY(EditAnywhere, NoClear, Category = Default)
	TSubclassOf<UObjectiveBase> DefaultObjectiveClass;

	UPROPERTY(EditAnywhere, NoClear, Category = UI)
	TSubclassOf<UObjectiveHUDWidget> DefaultHUDClass;
	UPROPERTY(EditAnywhere, NoClear, Category = UI)
	TSubclassOf<UObjectiveWidget> DefaultObjectiveWidgetClass;
	
	UPROPERTY(Transient)
	UObjectiveHUDWidget* HUDInstance;

	UPROPERTY(ReplicatedUsing = OnRep_ObjectiveList)
	TArray<UObjectiveBase*> ObjectiveList;

	UObjectiveBase* FindObjectiveFromSettings(UObjectiveSettings* ObjectiveSettings);

protected:
	UObjectiveBase* Internal_CreateObjective(UObjectiveSettings* ObjectiveSettings, UObjectiveBase* ParentObjective = nullptr);

	virtual void InitHUDWidget();

private:
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_AddObjective(UObjectiveBase* Objective);

	TArray<FObjectiveDelegateContainer> OnStartDelegateContainers;
	TArray<FObjectiveDelegateContainer> OnCompleteDelegateContainers;
	TArray<FObjectiveDelegateContainer> OnFailDelegateContainers;

	void Internal_BroadcastDelegates(UObjectiveSettings* Objective, TArray<FObjectiveDelegateContainer>& DelegatesToBroadcast);
	void Internal_CreateDelegate(UObjectiveSettings* Objective, const FObjectiveCallbackSignature& Delegate, TArray<FObjectiveDelegateContainer>& DelegateList);
};
