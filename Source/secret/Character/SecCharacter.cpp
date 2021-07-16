#include "SecCharacter.h"
#include "Capability/SecCapabilitySheet.h"
#include "SecCharacterMovementComponent.h"
#include "Components/InputComponent.h"
#include "Attribute/SecAttributeComponent.h"
#include "Capability/SecCapabilitySystemComponent.h"
#include "SecGameplayAction.h"
#include "GameFramework/InputSettings.h"
#include "Components/SecInputComponent.h"

const FName ASecCharacter::CapabilitySystemName = FName(TEXT("AttributeComponent"));
const FName ASecCharacter::AttributeComponentName = FName(TEXT("CapabilitySystemComponent"));
const FName ASecCharacter::SecInputComponentName = FName(TEXT("SecInputComponent"));

ASecCharacter::ASecCharacter(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<USecCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	SecInputComponent = ObjectInitializer.CreateDefaultSubobject<USecInputComponent>(this, SecInputComponentName);
	Attribute = ObjectInitializer.CreateDefaultSubobject<USecAttributeComponent>(this, AttributeComponentName);
	CapabilitySystem = ObjectInitializer.CreateDefaultSubobject<USecCapabilitySystemComponent>(this, CapabilitySystemName);

	// Don't need this, ever. We set the controller from capability sheets.
	AIControllerClass = nullptr;
}

void ASecCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	TArray<FName> InputBindings;

	const UInputSettings* DefaultInputSettings = GetDefault<UInputSettings>();
	DefaultInputSettings->GetActionNames(InputBindings);

	const FName InputPressedFunctionName(TEXT("InputPressed"));
	const FName InputReleasedFunctionName(TEXT("InputReleased"));

	for (FName ActionName : InputBindings)
	{	
		FInputActionBinding PressedActionBinding(ActionName, IE_Pressed);
		FInputActionBinding ReleasedActionBinding(ActionName, IE_Released);
		
		FInputActionHandlerSignature PressedHandler;
		PressedHandler.BindUFunction(this, InputPressedFunctionName, ActionName);

		FInputActionHandlerSignature ReleasedHandler;
		ReleasedHandler.BindUFunction(this, InputReleasedFunctionName, ActionName);
		
		PressedActionBinding.ActionDelegate = PressedHandler;
		ReleasedActionBinding.ActionDelegate = ReleasedHandler;
		 
		PlayerInputComponent->AddActionBinding(PressedActionBinding);
		PlayerInputComponent->AddActionBinding(ReleasedActionBinding);
		SecInputComponent->InputAction.FindOrAdd(ActionName) = ESecActionState::Inactive;
	}

	InputBindings.Reset();
	const FName HandleAxisFunctionName(TEXT("InputReleased"));

	DefaultInputSettings->GetAxisNames(InputBindings);

	for (FName AxisName : InputBindings)
	{
		FInputAxisBinding AxisBinding(AxisName);
		FInputAxisUnifiedDelegate AxisHandler;
		AxisHandler.GetDelegateForManualSet().BindLambda(
			[=](float Value)
			{
				HandleAxisInput(Value, AxisName);
			}
		);
		AxisBinding.AxisDelegate = AxisHandler;
		PlayerInputComponent->AxisBindings.Add(AxisBinding);
		SecInputComponent->InputAxis.FindOrAdd(AxisName) = 0.0f;
	}

	SecInputComponent->bInitialized = true;
}

USecCharacterMovementComponent* ASecCharacter::GetSecCharacterMovementComponent() const
{
	return CastChecked<USecCharacterMovementComponent>(GetMovementComponent());
}

void ASecCharacter::InputPressed(FName InputAction)
{
	SecInputComponent->InputAction.FindOrAdd(InputAction) = ESecActionState::Active;
}

void ASecCharacter::InputReleased(FName InputAction)
{
	SecInputComponent->InputAction.FindOrAdd(InputAction) = ESecActionState::Inactive;
}

void ASecCharacter::HandleAxisInput(float Value, FName AxisName)
{
	SecInputComponent->InputAxis.FindOrAdd(AxisName) = Value;
}

void ASecCharacter::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	if (TeamID != NewTeamID)
	{
		TeamID = NewTeamID;
		// @todo notify perception system that a controller changed team ID
	}
}
