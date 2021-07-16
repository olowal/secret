#include "SecPlayerControllerBase.h"

ASecPlayerControllerBase::ASecPlayerControllerBase(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{

}

void ASecPlayerControllerBase::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	if (IsLocalController())
	{
		BP_LocalOnPossessedPawn();
	}
}

void ASecPlayerControllerBase::OnRep_Pawn()
{
	Super::OnRep_Pawn();

	if (IsLocalController())
	{
		BP_LocalOnPossessedPawn();
	}
}

