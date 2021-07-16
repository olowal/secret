#pragma once

#include "Character/SecCharacter.h"
#include "SecPlayerCharacter.generated.h"

class USecTriggerUserComponent;

UCLASS(abstract)
class SECRET_API ASecPlayerCharacter : public ASecCharacter
{
	GENERATED_BODY()
public:
	ASecPlayerCharacter(const FObjectInitializer& ObjectInitializer);

protected:
	UPROPERTY(VisibleDefaultsOnly, Category = Trigger)
	USecTriggerUserComponent* TriggerUserComponent = nullptr;
};
