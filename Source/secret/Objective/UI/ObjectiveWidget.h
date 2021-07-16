#pragma once

#include "Blueprint/UserWidget.h"
#include "ObjectiveUIStatics.h"
#include "ObjectiveWidget.generated.h"

class UObjectiveSettings;
class UObjectiveBase;

UCLASS()
class SECRET_API UObjectiveWidget : public UUserWidget
{
	GENERATED_BODY()
public:

	virtual void SetupObjective(const FObjectiveUIInfo& UIInfo);
	virtual void OnProgress(const FObjectiveUIInfo& UIInfo);
	virtual void OnTimerTick(const FObjectiveUIInfo& UIInfo);
	virtual void OnObjectiveComplete(const FObjectiveUIInfo& UIInfo);
	virtual void OnObjectiveFail(const FObjectiveUIInfo& UIInfo);

	UFUNCTION(BlueprintCallable, Category = Objective)
	virtual void AddChildObjective(UObjectiveWidget* WidgetInstance);
	UFUNCTION(BlueprintImplementableEvent, Category = Objective, meta = (DisplayName = "On Add Child Objective"))
	void BP_OnAddChildObjective(UObjectiveWidget* WidgetInstance);

	// The first event to be called.
	UFUNCTION(BlueprintImplementableEvent, Category = Objective, meta = (DisplayName = "Setup Objective"))
	void BP_SetupObjective(const FObjectiveUIInfo& UIInfo);
	
	// Called whan a counter has been updated.
	UFUNCTION(BlueprintImplementableEvent, Category = Objective, meta = (DisplayName = "On Progress"))
	void BP_OnProgress(const FObjectiveUIInfo& UIInfo);

	// Ticks every time a timer ticks.
	UFUNCTION(BlueprintImplementableEvent, Category = Objective, meta = (DisplayName = "On Timer Tick"))
	void BP_OnTimerTick(const FObjectiveUIInfo& UIInfo);

	UFUNCTION(BlueprintImplementableEvent, Category = Objective, meta = (DisplayName = "On Objective Complete"))
	void BP_OnObjectiveComplete(const FObjectiveUIInfo& UIInfo);

	UFUNCTION(BlueprintImplementableEvent, Category = Objective, meta = (DisplayName = "On Objective Fail"))
	void BP_OnObjectiveFail(const FObjectiveUIInfo& UIInfo);
};
