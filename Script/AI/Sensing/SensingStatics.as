
namespace SensingStatics
{
	bool IsPointInsideCone2D(FVector Origin, FVector Direction, FVector TargetPoint, float Angle, float Distance)
	{
		const float DistanceSq = FMath::Square(Distance);

		if(TargetPoint.DistSquared2D(Origin) > DistanceSq)
			return false;

		const float Radians = FMath::Cos(FMath::DegreesToRadians(Angle));
		const FVector DirectionToTarget = (TargetPoint - Origin).GetSafeNormal2D();
		const float Dot = Direction.DotProduct(DirectionToTarget);
		bool bIsInside = Dot > Radians;
		return bIsInside;
	}
}
