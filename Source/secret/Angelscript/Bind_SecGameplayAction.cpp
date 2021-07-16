#include "AngelscriptBinds.h"
#include "GameFramework/InputSettings.h"
#include "SecGameplayAction.h"
#include "Capability/SecCapabilityTags.h"

static void BindGameplayAction(const FName& ValueName, const FString& VariableName)
{
	FString Decleration = "const FName ";
	Decleration += VariableName;
	FAngelscriptBinds::BindGlobalVariable(TCHAR_TO_ANSI(*Decleration), &ValueName);
}

FAngelscriptBinds::FBind Bind_SecGameplayAction(FAngelscriptBinds::EOrder::Normal, []
{
	static TArray<FName> GlobalActionMapping;
	static TArray<FName> GlobalAxisMapping;
	const UInputSettings* DefaultInputSettings = GetDefault<UInputSettings>();

	{
		const FString nsString(TEXT("GameplayAction"));
		FAngelscriptBinds::FNamespace ns(TCHAR_TO_ANSI(*nsString));
		
		DefaultInputSettings->GetActionNames(GlobalActionMapping);

		for (const FName& InputAction : GlobalActionMapping)
		{
			BindGameplayAction(InputAction, InputAction.ToString());
		}

		BindGameplayAction(FSecGameplayAction::LeftAxisRaw, "LeftAxisRaw");
		BindGameplayAction(FSecGameplayAction::RightAxisRaw, "RightAxisRaw");
		BindGameplayAction(FSecGameplayAction::MovementDirection, "MovementDirection");
	}

	{
		const FString nsString(TEXT("SecAxisInput"));
		FAngelscriptBinds::FNamespace ns(TCHAR_TO_ANSI(*nsString));

		DefaultInputSettings->GetAxisNames(GlobalAxisMapping);

		for (const FName& InputAxis : GlobalAxisMapping)
		{
			BindGameplayAction(InputAxis, InputAxis.ToString());
		}
	}

	{
		const FString nsString(TEXT("CapabilityTags"));
		FAngelscriptBinds::FNamespace ns(TCHAR_TO_ANSI(*nsString));

		for (const FName& CapabilityTag : FSecCapabilityTags::Tags)
		{
			BindGameplayAction(CapabilityTag, CapabilityTag.ToString());
		}
	}
});