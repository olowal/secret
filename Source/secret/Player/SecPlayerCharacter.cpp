#include "SecPlayerCharacter.h"
#include "Components/SecTriggerUserComponent.h"

ASecPlayerCharacter::ASecPlayerCharacter(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	TriggerUserComponent = ObjectInitializer.CreateDefaultSubobject<USecTriggerUserComponent>(this, TEXT("TriggerUserComponent"));
}
