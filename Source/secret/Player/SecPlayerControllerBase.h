#pragma once

#include "GameFramework/PlayerController.h"
#include "SecPlayerControllerBase.generated.h"

UCLASS(abstract)
class SECRET_API ASecPlayerControllerBase : public APlayerController
{
	GENERATED_BODY()
public:
	ASecPlayerControllerBase(const FObjectInitializer& ObjectInitializer);

	virtual void OnPossess(APawn* aPawn) override;

	virtual void OnRep_Pawn() override;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = LocalOnPossessedPawn))
	void BP_LocalOnPossessedPawn();

	class ASEThirdPersonCamera* ThirdPersonCamera = nullptr;
};
