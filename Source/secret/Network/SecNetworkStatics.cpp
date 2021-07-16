#include "SecNetworkStatics.h"
#include "GameFramework/Pawn.h"

bool USecNetworkStatics::IsLocallyControlled(AActor* TargetActor)
{
	if (!ensure(TargetActor != nullptr))
		return false;

	if (const APawn* Pawn = Cast<APawn>(TargetActor))
	{
		return Pawn->IsLocallyControlled();
	}

	return TargetActor->HasAuthority();
}

bool USecNetworkStatics::UsePredictionForActor(AActor* TargetActor)
{
	if (!ensure(TargetActor != nullptr))
		return false;

	if (const APawn* Pawn = Cast<APawn>(TargetActor))
	{
		return Pawn->IsLocallyControlled() && !Pawn->HasAuthority();
	}

	return false;
}
