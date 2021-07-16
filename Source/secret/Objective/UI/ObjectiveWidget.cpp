#include "ObjectiveWidget.h"

void UObjectiveWidget::SetupObjective(const FObjectiveUIInfo& UIInfo)
{
	BP_SetupObjective(UIInfo);
}

void UObjectiveWidget::OnProgress(const FObjectiveUIInfo& UIInfo)
{
	BP_OnProgress(UIInfo);
}

void UObjectiveWidget::OnTimerTick(const FObjectiveUIInfo& UIInfo)
{
	BP_OnTimerTick(UIInfo);
}

void UObjectiveWidget::OnObjectiveComplete(const FObjectiveUIInfo& UIInfo)
{
	BP_OnObjectiveComplete(UIInfo);
}

void UObjectiveWidget::OnObjectiveFail(const FObjectiveUIInfo& UIInfo)
{
	BP_OnObjectiveFail(UIInfo);
}

void UObjectiveWidget::AddChildObjective(UObjectiveWidget* WidgetInstance)
{
	BP_OnAddChildObjective(WidgetInstance);
}
