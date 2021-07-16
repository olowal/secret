#pragma once

#include "GameFramework/Actor.h"
#include "SecThirdPersonCameraBase.generated.h"

UCLASS(abstract)
class SECRET_API ASecThirdPersonCameraBase : public AActor
{
	GENERATED_BODY()
public:
	ASecThirdPersonCameraBase(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(Transient, BlueprintReadWrite, Category = "SE|ThirdPersonCamera")
	AActor* TargetActor = nullptr;
};
