#include "SecUserWidget.h"
#include "Components/WidgetComponent.h"

void USecUserWidget::SetVisibility(ESlateVisibility InVisibility)
{
	Super::SetVisibility(InVisibility);

	if (WidgetComponent)
	{
		if (InVisibility == ESlateVisibility::Collapsed || InVisibility == ESlateVisibility::Hidden)
		{
			WidgetComponent->SetVisibility(false);
		}
		else
		{
			WidgetComponent->SetVisibility(true);
		}
	}
}

void USecUserWidget::RemoveFromParent()
{
	DestroyWidgetComponent();

	Super::RemoveFromParent();
}

void USecUserWidget::DestroyWidgetComponent()
{
	if (WidgetComponent)
	{
		WidgetComponent->DestroyComponent();
	}
}

UObject* USecUserWidget::GetObjectOwner() const
{
	if (UUserWidget* OwningUserWidget = GetTypedOuter<UUserWidget>())
	{
		return OwningUserWidget->GetOuter();
	}

	return WidgetComponent->GetOwner();
}
