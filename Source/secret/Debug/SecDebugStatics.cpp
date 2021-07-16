#include "SecDebugStatics.h"
#include "SceneManagement.h"
#include "DrawDebugHelpers.h"

void USecDebugStatics::DrawLine(UWorld* World, const FVector& Start, const FVector& End, const FLinearColor& Color, float Thickness, bool bPersistentLines, float Lifetime, FPrimitiveDrawInterface* PDI)
{
	if (PDI)
	{
		PDI->DrawLine(Start, End, Color, SDPG_World, Thickness);
	}
	else
	{
		DrawDebugLine(World, Start, End, Color.ToFColor(false), bPersistentLines, Lifetime, SDPG_World, Thickness);
	}
}
