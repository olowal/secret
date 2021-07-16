#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "SecUIStatics.generated.h"

class USecUserWidget;
class ASecPlayerCharacter;
class UWidgetComponent;

UCLASS()
class SECRET_API USecUIStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, Category = "UI")
	static USecUserWidget* AttachWidgetToComponent(TSubclassOf<USecUserWidget> WidgetClass, USceneComponent* AttachComponent, UObject* OwningObject, EWidgetSpace WidgetSpace);

	UFUNCTION(BlueprintCallable, Category = "UI")
	static USecUserWidget* AttachWidgetToActor(TSubclassOf<USecUserWidget> WidgetClass, AActor* AttachActor, UObject* OwningObject, EWidgetSpace WidgetSpace);

	static USecUserWidget* CreateWidget(AActor* OwningObject, TSubclassOf<UUserWidget> UserWidgetClass, FName WidgetName = NAME_None);

private: 
	static USecUserWidget* CreateWidgetComponent(TSubclassOf<USecUserWidget> WidgetClass, UObject* OwningObject, USceneComponent* AttachComponent, EWidgetSpace WidgetSpace);
};
