import AI.Perception.SecSensingComponent;
import AI.Perception.PerceptionTargetInfo;

event void FOnTargetSeen(USecSensingComponent SensingComponent);

class USecAIPerceptionComponent : UActorComponent
{
	TArray<AActor> CachedIgnoreActors;

	private USceneComponent _OriginTransform = nullptr;

	void SetOriginTransform(USceneComponent InOriginTransform)
	{
		_OriginTransform = InOriginTransform;
	}

	UPROPERTY()
	float SightRadius = 2000.0f;

	float GetSightRadiusSq() const property { return FMath::Square(SightRadius); }

	/** How far to the side AI can see, in degrees. Use SetPeripheralVisionAngle to change the value at runtime. */
	UPROPERTY(meta = (DisplayName = "PeripheralVisionAngle", ClampMax = 179, ClampMin = 10))
	protected float _PeripheralVisionAngle = 110.0f;	// Should be from -180 to 180.
	protected float _PeripheralVisionCosine = 0;

	UPROPERTY()
	FOnTargetSeen OnTargetSeen;

	UPROPERTY()
	FOnTargetSeen OnTargetLost;

	// Contains available instances of UTargetPerceptionInfo.
	private TArray<UPerceptionTargetInfo> CachedTargetCollection;
	// Visible targets.
	private TArray<UPerceptionTargetInfo> TargetCollection;

	private TArray<UPerceptionTargetInfo> CachedRemoveCollection;

	UFUNCTION(BlueprintOverride)
	void BeginPlay()
	{
		CachedIgnoreActors.Add(Owner);
		SetPeripheralVisionAngle(_PeripheralVisionAngle);
	}

	void SetPeripheralVisionAngle(float InPeripheralVisionAngle)
	{
		_PeripheralVisionAngle = InPeripheralVisionAngle;
		_PeripheralVisionCosine = FMath::Cos(FMath::DegreesToRadians(_PeripheralVisionAngle * 0.5f));
	}

	UFUNCTION(BlueprintOverride)
	void Tick(float DeltaTime)
	{
		UpdateForgetTargets(DeltaTime);
		System::DrawDebugConeInDegrees(OriginLocation, OriginForwardVector, SightRadius, _PeripheralVisionAngle * 0.5f, 10.0f, 12, FLinearColor::Green);
	}

	private UPerceptionTargetInfo GetFree()
	{
		if(CachedTargetCollection.Num() == 0)
		{
			UPerceptionTargetInfo NewPerceptionInfo = Cast<UPerceptionTargetInfo>(NewObject(Owner, UPerceptionTargetInfo::StaticClass()));
			CachedTargetCollection.Add(NewPerceptionInfo);
		}

		UPerceptionTargetInfo PerceptionInfo = CachedTargetCollection.Last();
		return PerceptionInfo;
	}

	private void ReturnInstance(UPerceptionTargetInfo Info)
	{
		Info.Actor = nullptr;
		Info.bVisible = false;
		Info.Aggro = 0;
		TargetCollection.Remove(Info);
		CachedTargetCollection.Add(Info);
	}

	int FindIndex(AActor Actor) const
	{
		for(int Index = 0, Num = TargetCollection.Num(); Index < Num; ++Index)
		{
			if(TargetCollection[Index].Actor == Actor)
				return Index;
		}

		return -1;
	}

	void Handle_OnTargetSeen(USecSensingComponent SensingComp)
	{
		AddOrUpdateTarget(SensingComp.Owner);
		OnTargetSeen.Broadcast(SensingComp);
	}

	void Handle_OnTargetLost(USecSensingComponent SensingComp)
	{
		ForgetTarget(SensingComp.Owner);
		OnTargetLost.Broadcast(SensingComp);
	}

	private void AddOrUpdateTarget(AActor Actor)
	{
		bool bFound = false;

		for(UPerceptionTargetInfo Info : TargetCollection)
		{
			if(Info.Actor == Actor)
			{
				Info.bVisible = true;
			}
		}

		if(!bFound)
		{
			UPerceptionTargetInfo Info = GetFree();
			Info.Actor = Actor;
			Info.bVisible = true;
			Info.Aggro = 0.5f;
			TargetCollection.Add(Info);
		}
	}

	private void ForgetTarget(AActor Actor)
	{
		int Index = FindIndex(Actor);

		if(Index < 0)
			return;

		UPerceptionTargetInfo Info = TargetCollection[Index];
		Info.bVisible = false;
		Info.LastLocation = Actor.ActorLocation;
	}

	private void UpdateForgetTargets(float DeltaTime)
	{
		CachedRemoveCollection.Reset();

		for(int Index = TargetCollection.Num() - 1; Index >= 0; --Index)
		{
			UPerceptionTargetInfo Info = TargetCollection[Index];

			if(!Info.bVisible)
			{
				Info.Aggro -= DeltaTime;

				if(Info.Aggro <= 0.f)
				{
					CachedRemoveCollection.Add(Info);
				}
			}
		}

		for(UPerceptionTargetInfo Info : CachedRemoveCollection)
		{
			ReturnInstance(Info);
		}
	}

	const UPerceptionTargetInfo GetTargetWithHighestAggro() const
	{
		UPerceptionTargetInfo TargetActor = nullptr;
		float HighestAggro = 0;

		for(UPerceptionTargetInfo Info : TargetCollection)
		{
			if(Info.Aggro > HighestAggro && Info.bVisible)
			{
				HighestAggro = Info.Aggro;
				TargetActor = Info;
			}
		}

		return TargetActor;
	}

	bool IsPointWithinPeripheralVision(FVector Point) const
	{
		const FVector DistanceTo = Point - OriginLocation;
		const float DistanceSq = DistanceTo.SizeSquared();

		if(DistanceSq > SightRadiusSq)
			return false;

		const FVector DirectionToPoint = DistanceTo.GetSafeNormal();
		const float Dot = DirectionToPoint.DotProduct(OriginForwardVector);
		return Dot > _PeripheralVisionCosine;
	}

	bool IsPointWithinPeripheralVision2D(FVector Point) const
	{
		const FVector DistanceTo = Point - OriginLocation;
		const float DistanceSq = DistanceTo.SizeSquared2D();

		if(DistanceSq > SightRadiusSq)
			return false;

		const FVector DirectionToPoint = DistanceTo.GetSafeNormal2D();
		const float Dot = DirectionToPoint.DotProduct(OriginForwardVector);
		return Dot > _PeripheralVisionCosine;
	}

	bool HasLineOfSightToActor(const AActor Target, FVector Point) const
	{
		if(Target == nullptr)
			return false;

		FHitResult Hit;
		const FVector StartLoc = OriginLocation;
		const FVector EndLoc = StartLoc + OriginForwardVector * SightRadius;
		System::LineTraceSingle(StartLoc, Target.ActorLocation, ETraceTypeQuery::Visibility, false, CachedIgnoreActors, EDrawDebugTrace::None, Hit, false);

		if(Hit.bBlockingHit && Hit.Actor == Target)
			return true;

		return false;
	}

	int NumTargets() const { return TargetCollection.Num(); }

	FVector GetOriginLocation() const property
	{
		if(_OriginTransform != nullptr)
			return _OriginTransform.WorldLocation;

		return FVector::ZeroVector;
	}

	FVector GetOriginForwardVector() const property
	{
		if(_OriginTransform != nullptr)
			return _OriginTransform.ForwardVector;

		return FVector::ForwardVector;
	}
}
