import Attack.CharacterAttackComponent;
import Equipment.MeleeWeaponComponent;
import Attribute.StrengthAttribute;
import Damage.DealDamageComponent;

/*
	This should only be activated after Hold has been deactivated
*/

class UCharacterAttackReleaseCapability : USecCapability
{
    default Tags.Add(n"CharacterAttack");

    ASecCharacter OwnerCharacter;
	UMeleeWeaponComponent WeaponComp;
	UDealDamageComponent DealDamage;

	UMeleeWeaponAttackSettings AttackSettings;
	TArray<AActor> HitActors;

    float Elapsed = 0.0f;
	float StartTime = 0.0f;

    default TickOrder = 102;

    UFUNCTION(BlueprintOverride)
    void Setup()
    {
        OwnerCharacter = Cast<ASecCharacter>(Owner);
		WeaponComp = UMeleeWeaponComponent::Get(Owner);
		DealDamage = UDealDamageComponent::Get(Owner);
    }

    UFUNCTION(BlueprintOverride)
    ECapabilityActivation ShouldActivate() const
    {
		if(WeaponComp.AttackState != EMeleeWeaponAttackState::Release)
			return ECapabilityActivation::DontActivate;

        if(WeaponComp.WeaponSettings == nullptr)
            return ECapabilityActivation::DontActivate;

		if(WeaponComp.AttackMontageCurrent == nullptr)
			return ECapabilityActivation::DontActivate;

        return ECapabilityActivation::Local;
    }

    UFUNCTION(BlueprintOverride)
    void OnActivate(FCapabilityActivationParams ActivationParams)
    {
		WeaponComp.NumRelease++;
		UAnimMontage MontageCurrent = WeaponComp.AttackMontageCurrent;
        OwnerCharacter.PlayAnimMontage(MontageCurrent, 1.0f, n"Attack");
        StartTime = Elapsed = MontageCurrent.GetSectionLengthByName(n"Attack") * 0.5f;
        HitActors.Empty();
        BlockCapabilities(n"CharacterFaceDirection", this);
        BlockCapabilities(n"CharacterBlock", this);

		TSubclassOf<USecAttribute> Attr = USecCharacterRotationSpeedMultiplierAttribute::StaticClass();
		Attribute.ApplyModifier(FSecAttributeModifierContainer(0.1f, Attr, ESecAttributeModifierType::Mul), this, ESecScriptPriority::High);
    }

    UFUNCTION(BlueprintOverride)
    ECapabilityDeactivation ShouldDeactivate() const
    {
	    if(Elapsed <= 0.0f)
            return ECapabilityDeactivation::Network;

        return ECapabilityDeactivation::DontDeactivate;
    }

    UFUNCTION(BlueprintOverride)
    void OnDeactivate(FCapabilityDeactivationParams& DeactivationParams)
    {
        UnblockCapabilities(n"CharacterFaceDirection", this);
        UnblockCapabilities(n"CharacterBlock", this);
		Attribute.RemoveModifierByInstigator(this);
		WeaponComp.AttackState = EMeleeWeaponAttackState::None;
    }

    UFUNCTION(BlueprintOverride)
    void TickActive(float DeltaTime)
    {
        if(!IsLocallyControlled())
            return;
        
        Elapsed -= DeltaTime;

        if(Elapsed < (StartTime * 0.8f))
        {
            TArray<AActor> ActorsToIgnore;
            ActorsToIgnore.Add(Owner);

            if(HitActors.Num() > 0)
            {
                ActorsToIgnore.Append(HitActors);
            }

            const float Length = 100.0f;
            const FVector StartLocation = WeaponComp.WorldLocation;
            const FVector EndLocation = StartLocation + (WeaponComp.ForwardVector * Length);
            const FVector HalfSize(Length * 0.1f, 10.0f, 10.0f);

            TArray<FHitResult> OutHits;
            System::BoxTraceMulti(StartLocation, EndLocation, HalfSize, WeaponComp.WorldRotation, ETraceTypeQuery::WeaponMelee, false, ActorsToIgnore, EDrawDebugTrace::None, OutHits, false);

			const FVector CenterLocation = StartLocation + (WeaponComp.ForwardVector * (Length * 0.5f));

			FDealDamageInfo DamageInfo;
			DamageInfo.DamageDealer = Owner;
			DamageInfo.Damage = 12;
			DamageInfo.Origin = Owner.ActorLocation;

            for(FHitResult& Hit : OutHits)
            {
                if(Hit.Actor == nullptr || HitActors.Contains(Hit.Actor))
                {
                    continue;
                }

                //Print(Hit.Actor.Name, 1.0f);
                HitActors.Add(Hit.Actor);
				DamageInfo.TargetActor = Hit.Actor;
				DealDamage.DamageToDeal.Add(DamageInfo);
            }
        }
    }

	UStrengthAttribute GetStrengthAttribute() const
	{
		UStrengthAttribute StrenghtAttr = Cast<UStrengthAttribute>(Attribute.GetOrCreate(UStrengthAttribute::StaticClass()));
		return StrenghtAttr;
	}

	UFUNCTION(BlueprintOverride)
	void PreTick(float DeltaTime)
	{
		//PrintToScreen("WeaponState: " + WeaponComp.AttackState);
	}
}
