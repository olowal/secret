#pragma once

#include "GameFramework/Actor.h"

#include "SecWeaponBase.generated.h"

UCLASS(Abstract)
class SECRET_API ASecWeaponBase : public AActor
{
	GENERATED_BODY()
public:
	ASecWeaponBase(const FObjectInitializer& ObjectInitializer);
};
