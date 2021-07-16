#include "ObjectiveHUDWidget.h"
#include "Objective/ObjectiveBase.h"
#include "ObjectiveWidget.h"

void UObjectiveHUDWidget::OnObjectiveStart(UObjectiveBase* Objective)
{
	if (UObjectiveWidget* ObjectiveWidget = Objective->SetupWidget())
	{
		ObjectiveWidget->AddToViewport();
		BP_OnObjectiveStart(ObjectiveWidget, Objective);
	}
}
