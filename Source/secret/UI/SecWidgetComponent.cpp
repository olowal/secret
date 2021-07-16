#include "SecWidgetComponent.h"
#include "SecUIStatics.h"
#include "SecUserWidget.h"
#include "Application/SlateApplication.h"
#include "ConstructorHelpers.h"
#include "Materials/MaterialInstance.h"

USecWidgetComponent::USecWidgetComponent()
{
	SetUsingAbsoluteRotation(true);

	static ConstructorHelpers::FObjectFinder<UMaterialInstance> FaceCameraTextMaterial(TEXT("/Game/Assets/Materials/Font/MI_FaceCamera"));
	
	UMaterialInstance* FaceCameraTextMaterialInstance = CastChecked<UMaterialInstance>(FaceCameraTextMaterial.Object, ECastCheckedType::NullAllowed);

	if (FaceCameraTextMaterialInstance != nullptr)
	{
		OverrideMaterials.Reset();
		OverrideMaterials.Add(FaceCameraTextMaterialInstance);
	}
}

void USecWidgetComponent::InitWidget()
{
	// Don't do any work if Slate is not initialized
	if (FSlateApplication::IsInitialized())
	{
		if (WidgetClass && GetWidget() == nullptr && GetWorld())
		{
			if (!ensure(WidgetClass.GetDefaultObject()->IsA(USecUserWidget::StaticClass())))
				return;

			SetWidget(USecUIStatics::CreateWidget(GetOwner(), WidgetClass));
		}

#if WITH_EDITOR
		if (GetWidget() != nullptr && !GetWorld()->IsGameWorld() && !bEditTimeUsable)
		{
			if (!GEnableVREditorHacks)
			{
				// Prevent native ticking of editor component previews
				GetWidget()->SetDesignerFlags(EWidgetDesignFlags::Designing);
			}
		}
#endif
	}
}
