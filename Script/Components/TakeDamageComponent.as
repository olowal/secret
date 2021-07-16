import Attribute.HealthAttribute;

struct FTakeDamageInfo
{
	FVector Origin;	// Where the damage originated from.

	FVector DirectionToDamage;	// Normalized direction from origin.

	AActor DamageInstigator;	// What instigated the damage

    AActor DamageDealer;	// What is dealing the damage

    float Damage;	// how much damage

    float Impulse;	// Possible knockback
}

 event void FOnDamageTaken(FTakeDamageInfo DamageInfo);

class UTakeDamageComponent : USecTakeDamageComponent
{
	default PrimaryComponentTick.bStartWithTickEnabled = true;
	USecAttributeComponent Attribute;

	AActor LastDamageDealer;
	AActor LastDamageInstigator;

	// Origin of the last damage that was received
	FVector LastDamageLocation;
	
	UFUNCTION(BlueprintOverride)
	void BeginPlay()
	{
		Attribute = USecAttributeComponent::Get(Owner);
		Health = Cast<UHealthAttribute>(Attribute.GetAttributeByClass(UHealthAttribute::StaticClass()));
		ensure(Health != nullptr);
		Health.OnModified.AddUFunction(this, n"Handle_HealthModified");
	}

	bool HasAuthority() const
	{
		return Owner.HasAuthority();
	}

	UFUNCTION()
	private void Handle_HealthModified(float NewValue, float OldValue)
	{
		if(HasAuthority())
		{
			if(!FMath::IsNearlyZero(OldValue) && FMath::IsNearlyZero(NewValue))
			{
				bIsAlive = false;
			}
			else if(FMath::IsNearlyZero(OldValue) && !FMath::IsNearlyZero(NewValue))
			{
				bIsAlive = true;
			}
		}
	}

    UPROPERTY()
    int CumulativeDamageMax = 1;    // Temp probably. think through a more detailed damage system with dots etc

    void TakeDamage(AActor DamageInstigator, AActor DamageDealer, FVector Origin, float Damage)
    {
        if(CumulatedDamage.Num() >= CumulativeDamageMax)
            return;
        
        FTakeDamageInfo DamageInfo;

        DamageInfo.DamageInstigator = DamageInstigator;

        DamageInfo.DamageDealer = DamageDealer;
        DamageInfo.Damage = Damage;
        OnDamageTaken.Broadcast(DamageInfo);
		LastDamageDealer = DamageDealer;
		LastDamageInstigator = DamageInstigator;
		LastDamageLocation = Origin;
        //CumulatedDamage.Add(DamageInfo);
		Health.ModifyBaseValue(-Damage);
    }

	bool HasPendingDamage() const
	{
		return CumulatedDamage.Num() > 0;
	}

	float GetHealthValue() const property
	{
		ensure(Health != nullptr);
		return Health.AttributeValue;
	}

	bool IsAlive() const
	{
		return bIsAlive;
	}

    UPROPERTY()
    FOnDamageTaken OnDamageTaken;

    TArray<FTakeDamageInfo> CumulatedDamage;

    UPROPERTY()
    UAnimMontage FlinchAnimation;

	protected UHealthAttribute Health;

    bool bCanTakeDamage = true;

	bool bIsAlive = true;
}
