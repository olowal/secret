#pragma once

#include "GameFramework/GameState.h"
#include "SecGameStateBase.generated.h"

UCLASS(abstract)
class SECRET_API ASecGameStateBase : public AGameState
{
	GENERATED_BODY()
public:
	ASecGameStateBase(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(Transient, BlueprintReadWrite, Category = "SE|GameState")
	class ASecThirdPersonCameraBase* ThirdPersonCamera = nullptr;
};
