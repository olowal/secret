#pragma once

#include "GameFramework/Actor.h"
#include "SecShieldBase.generated.h"

UCLASS(Abstract)
class SECRET_API ASecShieldBase : public AActor
{
	GENERATED_BODY()
public:
	ASecShieldBase(const FObjectInitializer& ObjectInitializer);
};
