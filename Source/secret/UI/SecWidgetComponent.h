#pragma once

#include "Components/WidgetComponent.h"
#include "SecWidgetComponent.generated.h"

UCLASS()
class SECRET_API USecWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()
public:
	USecWidgetComponent();

	virtual void InitWidget() override;
};
