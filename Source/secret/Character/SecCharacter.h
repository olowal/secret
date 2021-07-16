#pragma once

#include "GameFramework/Character.h"
#include "GenericTeamAgentInterface.h"
#include "SecCharacter.generated.h"

class USecCapabilitySheet;
class USecInputComponent;

UCLASS(abstract)
class SECRET_API ASecCharacter : public ACharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()
public:
	ASecCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintPure, Category = Character)
	class USecCharacterMovementComponent* GetSecCharacterMovementComponent() const;

	UFUNCTION(BlueprintPure, Category = Character)
	class USecAttributeComponent* GetAttributeComponent() const { return Attribute; }

	UFUNCTION(BlueprintPure, Category = Character)
	class USecCapabilitySystemComponent* GetActionSystemComponent() const { return CapabilitySystem; }

	static const FName CapabilitySystemName;
	static const FName AttributeComponentName;
	static const FName SecInputComponentName;

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Action)
	class USecCapabilitySystemComponent* CapabilitySystem = nullptr;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite = Attribute)
	class USecAttributeComponent* Attribute = nullptr;

	UPROPERTY()
	class USecInputComponent* SecInputComponent = nullptr;

	UFUNCTION()
	void InputPressed(FName InputAction);

	UFUNCTION()
	void InputReleased(FName InputAction);

	UFUNCTION()
	void HandleAxisInput(float Value, FName AxisName);

	//----------------------------------------------------------------------//
	// IGenericTeamAgentInterface
	//----------------------------------------------------------------------//
private:
	FGenericTeamId TeamID;
public:
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override { return TeamID; }
};
