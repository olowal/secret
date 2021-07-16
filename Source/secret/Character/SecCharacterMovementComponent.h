#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "SecCharacterMovementComponent.generated.h"

class USecCharacterMovementSpeedMultiplierAttribute;
class USecCharacterRotationSpeedMultiplierAttribute;
class USecAttributeModifierSheet;
class ASecCharacter;

UCLASS()
class SECRET_API USecCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
public:

	const static FName CharacterMovementSpeedMultiplierName;

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void PhysicsRotation(float DeltaTime) override;

	virtual float GetMaxSpeed() const override;
	float GetMaxRotationSpeed() const;

	virtual bool IsMovingOnGround() const override;

	UFUNCTION(BlueprintCallable, Category = "Sec|CharacterMovement")
	void SetTargetFacingRotation(const FQuat& NewTargetFacingRotation);
	UFUNCTION(BlueprintCallable, Category = "Sec|CharacterMovement")
	void SetTargetFacingDirection(const FVector& NewTargetFacingDirection);

	UPROPERTY(Transient)
	class USecAttributeComponent* Attribute = nullptr;

	UPROPERTY(Category = "Character Movement (Rotation Settings)", EditAnywhere, BlueprintReadWrite)
	bool bFlipOrientRotationToMovement = false;

	UFUNCTION(BlueprintCallable, Category = "Sec|CharacterMovement")
	void ApplyKnockback(const FVector& InKnockbackImpulse);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Knockback, meta = (ClampMin = 0.0, ClampMax = 1.0))
	float KnockbackFriction = 0.8f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Knockback)
	float KnockbackStopSize = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement (General Settings)")
	float RotationSpeed = 12.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Sec|CharacterMovement")
	bool bKnockback = false;

	UFUNCTION(Server, Reliable)
	void Server_ApplyKnockback(const FVector& InKnockbackImpulse);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ApplyKnockback(const FVector& InKnockbackImpulse);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Movement)
	USecAttributeModifierSheet* SprintModifierSheet = nullptr;

protected:
	void Internal_ApplyKnockback(const FVector& InKnockbackImpulse);

	FVector KnockbackImpulse;

	FQuat TargetFacingDirection;
	FQuat CurrentFacingDirection;

	virtual void PhysCustom(float DeltaTime, int32 Iterations) override;

	virtual void ApplyVelocityBraking(float DeltaTime, float Friction, float BrakingDeceleration) override;

	virtual void ReplicateMoveToServer(float DeltaTime, const FVector& NewAcceleration) override;

	UPROPERTY(Transient)
	ASecCharacter* SecCharacterOwner = nullptr;

	UPROPERTY(Transient)
	USecCharacterMovementSpeedMultiplierAttribute* MovementSpeedMultipler = nullptr;

	UPROPERTY(Transient)
	USecCharacterRotationSpeedMultiplierAttribute* RotationSpeedMultiplier = nullptr;
};
