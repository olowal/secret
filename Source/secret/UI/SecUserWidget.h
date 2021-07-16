#pragma once

#include "Blueprint/UserWidget.h"
#include "SecUserWidget.generated.h"

class UWidgetComponent;
class AActor;

UCLASS()
class SECRET_API USecUserWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void SetVisibility(ESlateVisibility InVisibility) override;

	virtual void RemoveFromParent() override;

	void DestroyWidgetComponent();

	UFUNCTION(BlueprintPure, Category = UI)
	UObject* GetObjectOwner() const;

	UPROPERTY(Transient)
	UWidgetComponent* WidgetComponent = nullptr;
};
