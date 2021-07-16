import Components.TakeDamageComponent;
import Components.AnimatedTextComponent;
import Attribute.HealthAttribute;

class UCharacterTakeDamageCapability : USecCapability
{
    default TickOrder = 10;
	
	FVector DirectionToDamage;
    ASecCharacter OwnerCharacter;
    UTakeDamageComponent TakeDamageComp;
    USecCharacterMovementComponent MoveComp;
	UHealthAttribute Health;

    UPROPERTY()
    UAnimatedTextData AnimatedTextData;

    float ElapsedTime = 0.0f;
	bool bAppliedKnockback = false;

    UFUNCTION(BlueprintOverride)
    void Setup()
    {
        OwnerCharacter = Cast<ASecCharacter>(Owner);
        TakeDamageComp = UTakeDamageComponent::Get(Owner);
        MoveComp = USecCharacterMovementComponent::Get(Owner);
		//Health = Attribute.GetAttr
    }

    UFUNCTION(BlueprintOverride)
    ECapabilityActivation ShouldActivate() const
    {
        if(!TakeDamageComp.bCanTakeDamage)
			return ECapabilityActivation::DontActivate;
		
		if(TakeDamageComp.CumulatedDamage.Num() == 0)
            return ECapabilityActivation::DontActivate;

        return ECapabilityActivation::Local;
    }

	UFUNCTION(BlueprintOverride)
	void PreActivation(FActivationSyncParams& SyncParams)
	{
		FTakeDamageInfo DamageInfo = TakeDamageComp.CumulatedDamage[0];
		
		AActor DamageDealer = DamageInfo.DamageDealer;
		float Damage = DamageInfo.Damage;

		SyncParams.AddObject(n"DamageDealer", DamageDealer);
		SyncParams.AddNumber(n"Damage", Damage);
		SyncParams.AddVector(n"DamageOrigin", DamageInfo.Origin);
	}

    UFUNCTION(BlueprintOverride)
    void OnActivate(FCapabilityActivationParams ActivationParams)
    {
        TakeDamageComp.CumulatedDamage.RemoveAt(0);
		bAppliedKnockback = false;
        ElapsedTime = 0.6f;
		OwnerCharacter.PlayAnimMontage(TakeDamageComp.FlinchAnimation);
		AActor DamageDealer = Cast<AActor>(ActivationParams.GetObject(n"DamageDealer"));
		float Damage = ActivationParams.GetNumber(n"Damage");
		const FVector DamageOrigin = ActivationParams.GetVector(n"DamageOrigin");
		
        DirectionToDamage = (Owner.ActorLocation - DamageOrigin).GetSafeNormal();

        Gameplay::CreateAnimatedText(Owner.ActorLocation, "" + int(Damage), AnimatedTextData, Owner);
    }

    UFUNCTION(BlueprintOverride)
    void OnDeactivate(FCapabilityDeactivationParams& DeactivationParams)
    {

    }

    UFUNCTION(BlueprintOverride)
    ECapabilityDeactivation ShouldDeactivate() const
    {
        return ECapabilityDeactivation::Local;
    }

    UFUNCTION(BlueprintOverride)
    void TickActive(float DeltaTime)
    {
        ElapsedTime -= DeltaTime;

		if(ElapsedTime < 0.45 && !bAppliedKnockback)
		{
			MoveComp.ApplyKnockback(DirectionToDamage * 1000.0f);
			bAppliedKnockback = true;
		}

        //System::DrawDebugArrow(Owner.ActorLocation, Owner.ActorLocation + (DirectionToInstigator * 100.0f), 10.0f, FLinearColor::Green, 0.5f);
        //FHitResult Hit;
        //Owner.AddActorWorldOffset(DirectionToInstigator * 6.0f, true, Hit, false);
    }
/*
    bool IsBlockValid(const FCapabilityEventParams& EventParams) const
    {
        const bool bIsBlocking = Attribute.GetActionAttribute(n"IsBlocking");

        if(!bIsBlocking)
        {
            return false;
        }

        UObject InstigatorObject;
        EventParams.Objects.Find(n"Instigator", InstigatorObject);
        AActor Instigator = Cast<AActor>(InstigatorObject);

        if(Instigator == nullptr)
        {
            return true;
        }

        const float BlockingAngle = 100.0f;
        const float BlockingRadius = FMath::Cos(FMath::DegreesToRadians(BlockingAngle * 0.5f));
        const FVector DirectionTo = (Instigator.ActorLocation - Owner.ActorLocation).GetSafeNormal();
        const float DirectionDot = DirectionTo.DotProduct(Owner.ActorForwardVector);

        return DirectionDot > BlockingRadius;
    }
*/
}
