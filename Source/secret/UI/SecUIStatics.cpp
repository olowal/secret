#include "SecUIStatics.h"
#include "SecUserWidget.h"
#include "SecWidgetComponent.h"

USecUserWidget* USecUIStatics::AttachWidgetToComponent(TSubclassOf<USecUserWidget> WidgetClass, USceneComponent* AttachComponent, UObject* OwningObject, EWidgetSpace WidgetSpace)
{
	if (!ensure(*WidgetClass != nullptr))
		return nullptr;
	if (!ensure(AttachComponent != nullptr))
		return nullptr;

	return CreateWidgetComponent(WidgetClass, OwningObject, AttachComponent, WidgetSpace);
}

USecUserWidget* USecUIStatics::AttachWidgetToActor(TSubclassOf<USecUserWidget> WidgetClass, AActor* AttachActor, UObject* OwningObject, EWidgetSpace WidgetSpace)
{
	if (!ensure(*WidgetClass != nullptr))
		return nullptr;
	if (!ensure(AttachActor != nullptr))
		return nullptr;

	return CreateWidgetComponent(WidgetClass, OwningObject, AttachActor->GetRootComponent(), WidgetSpace);
}

USecUserWidget* USecUIStatics::CreateWidget(AActor* OwningObject, TSubclassOf<UUserWidget> UserWidgetClass, FName WidgetName)
{
	if (OwningObject)
	{
		return CastChecked<USecUserWidget>(UUserWidget::CreateWidgetInstance(OwningObject, UserWidgetClass, WidgetName));
	}
	return nullptr;
}

USecUserWidget* USecUIStatics::CreateWidgetComponent(TSubclassOf<USecUserWidget> WidgetClass, UObject* OwningObject, USceneComponent* AttachComponent, EWidgetSpace WidgetSpace)
{
	UWidgetComponent* WidgetComponent = NewObject<USecWidgetComponent>(OwningObject, USecWidgetComponent::StaticClass());
	if (!ensure(WidgetComponent != nullptr))
		return nullptr;

	WidgetComponent->SetWidgetSpace(WidgetSpace);
	WidgetComponent->SetWidgetClass(WidgetClass);
	WidgetComponent->SetupAttachment(AttachComponent);
	WidgetComponent->RegisterComponent();
	USecUserWidget* UserWidget = CastChecked<USecUserWidget>(WidgetComponent->GetUserWidgetObject());
	UserWidget->WidgetComponent = WidgetComponent;
	return UserWidget;
}
