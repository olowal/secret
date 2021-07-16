#pragma once

#include "Blueprint/UserWidget.h"
#include "ObjectiveHUDWidget.generated.h"

class UObjectiveWidget;
class UObjectiveBase;

UCLASS()
class SECRET_API UObjectiveHUDWidget : public UUserWidget
{
	GENERATED_BODY()
public:

	virtual void OnObjectiveStart(UObjectiveBase* Objective);

	UFUNCTION(BlueprintImplementableEvent, Category = Objective, meta = (DisplayName = "On Objective Start"))
	void BP_OnObjectiveStart(UObjectiveWidget* WidgetInstance, UObjectiveBase* Objective);

};
