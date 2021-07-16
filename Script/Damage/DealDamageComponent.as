import Components.AnimatedTextComponent;

struct FDealDamageInfo
{
	FVector Origin;	// The location that the damage originated from
	FVector DirectionToDamage;	// Normalized direction to damage origin
	AActor DamageDealer;	// who is dealing the damage
	AActor TargetActor;	// who should be damaged?
	float Damage;	// how much damage to deal
}

class UDealDamageComponent : UActorComponent
{
    TArray<FDealDamageInfo> DamageToDeal;

	UPROPERTY()
	UAnimatedTextData TextData;
}
