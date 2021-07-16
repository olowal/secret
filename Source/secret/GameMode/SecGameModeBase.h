#pragma once

#include "GameFramework/GameModeBase.h"
#include "SecGameModeBase.generated.h"

UCLASS(abstract)
class SECRET_API ASecGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	ASecGameModeBase(const FObjectInitializer& ObjectInitializer);
};
