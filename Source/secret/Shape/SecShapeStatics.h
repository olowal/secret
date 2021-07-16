#pragma once

#include "Components/PrimitiveComponent.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SecShapeStatics.generated.h"

UENUM(BlueprintType)
enum class ESecShapeType : uint8
{
	Sphere,
	Box,
	Capsule,
	None
};

USTRUCT(BlueprintType)
struct FSecShapeSettings
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESecShapeType Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "Type == ESecShapeType::Box", EditConditionHides))
	FVector BoxExtends = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "Type == ESecShapeType::Sphere", EditConditionHides))
	float SphereRadius = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "Type == ESecShapeType::Capsule", EditConditionHides))
	float CapsuleHalfHeight = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "Type == ESecShapeType::Capsule", EditConditionHides))
	float CapsuleRadius = 0.f;
};

UCLASS()
class SECRET_API USecShapeStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	/* Create a primitive component of the specified shape and transform. */
	UFUNCTION(BlueprintCallable, Category = "Shapes")
	static UPrimitiveComponent* CreateTriggerShape(AActor* InActor, USceneComponent* AttachTo, const FSecShapeSettings& Settings, const FTransform& Transform, const FName& CollisionProfile = NAME_None);

	/* Update settings for an already created primitive component. */
	UFUNCTION(BlueprintCallable, Category = "Shapes")
	static void SetPrimitiveSize(UPrimitiveComponent* PrimitiveComponent, const FSecShapeSettings& Settings);

private:
	static UPrimitiveComponent* CreateShapeInternal(AActor* InActor, const FSecShapeSettings& Settings, const FTransform& Transform);
};
