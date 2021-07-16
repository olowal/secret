import Damage.DealDamageComponent;
import Components.TakeDamageComponent;
import Components.HitStaggerComponent;
import Components.KnockbackComponent;

class UDealDamageCapability : USecCapability
{
	UDealDamageComponent DealDamage;
	UHitStaggerComponent HitStagger;
	
	UFUNCTION(BlueprintOverride)
	void Setup()
	{
		DealDamage = UDealDamageComponent::Get(Owner);
		HitStagger = UHitStaggerComponent::Get(Owner);
	}

	UFUNCTION(BlueprintOverride)
	ECapabilityActivation ShouldActivate() const
	{
		if(DealDamage.DamageToDeal.Num() == 0)
			return ECapabilityActivation::DontActivate;

		return ECapabilityActivation::Network;
	}

    UFUNCTION(BlueprintOverride)
    void PreActivation(FActivationSyncParams& SyncParams)
    {
		FDealDamageInfo DamageInfo = DealDamage.DamageToDeal.Last();
		DealDamage.DamageToDeal.RemoveAt(DealDamage.DamageToDeal.Num() - 1);

		SyncParams.AddObject(n"TargetActor", DamageInfo.TargetActor);
		SyncParams.AddVector(n"DamageOrigin", DamageInfo.Origin);
		SyncParams.AddNumber(n"Damage", DamageInfo.Damage);
    }
/*
	UFUNCTION(BlueprintOverride)
	void Prediction_OnActivate(FCapabilityActivationParams ActivationParams)
	{
		ACharacter TargetActor = Cast<ACharacter>(ActivationParams.GetObject(n"TargetActor"));

		if(TargetActor == nullptr)
			return;

		UTakeDamageComponent TakeDamageComp = UTakeDamageComponent::Get(TargetActor);

		if(TakeDamageComp == nullptr)
			return;

		if(!TakeDamageComp.IsAlive())
			return;

		// Let's predict damage calculation
		float Damage = ActivationParams.GetNumber(n"Damage");
		const FVector DamageOrigin = ActivationParams.GetVector(n"DamageOrigin");

		FSecDamageInput Input(Damage, DamageOrigin, Owner);
		FSecDamageOutput Output;

		TakeDamageComp.ProcessDamage(Input, Output);

		if(!FMath::IsNearlyZero(Output.Damage))
		{
			TargetActor.PlayAnimMontage(TakeDamageComp.FlinchAnimation);
		}
		else
		{
			HitStagger.bHitStagger = true;
		}
	}
	*/

	UFUNCTION(BlueprintOverride)
	EServerCapabilityActivation Server_ConfirmActivation(FCapabilityActivationParams ActivationParams, FActivationSyncParams& SyncParams)
	{
		AActor TargetActor = Cast<AActor>(ActivationParams.GetObject(n"TargetActor"));
		float Damage = ActivationParams.GetNumber(n"Damage");
		const FVector DamageOrigin = ActivationParams.GetVector(n"DamageOrigin");

		FSecDamageInput Input(Damage, DamageOrigin, Owner);
		FSecDamageOutput Output;

		UTakeDamageComponent TakeDamageComp = UTakeDamageComponent::Get(TargetActor);

		if(TakeDamageComp == nullptr)
		{
			PrintToScreen("No Take Damage Component in " + TargetActor.Name);
			return EServerCapabilityActivation::DontActivate;
		}

		if(!TakeDamageComp.IsAlive())
			return EServerCapabilityActivation::DontActivate;

		//TakeDamageComp.ProcessDamage(Input, Output);

		const float HealthCurrent = TakeDamageComp.HealthValue;
		if((HealthCurrent - Output.DamageDealt) <= 0.0f)
		{
			SyncParams.AddActionState(n"Killshot");
		}

		SyncParams.AddNumber(n"Damage", Output.DamageDealt);
		const FVector DirectionToDamage;

		return EServerCapabilityActivation::Activate;
	}

	UFUNCTION(BlueprintOverride)
	void OnActivate(FCapabilityActivationParams ActivationParams)
	{
		const bool bKillshot = ActivationParams.GetActionState(n"Killshot");
		AActor TargetActor = Cast<AActor>(ActivationParams.GetObject(n"TargetActor"));
		float Damage = ActivationParams.GetNumber(n"Damage");
		const FVector DamageOrigin = ActivationParams.GetVector(n"DamageOrigin");

		UTakeDamageComponent TakeDamageComp = UTakeDamageComponent::Get(TargetActor);

		const FVector DirectionToDamage = (TargetActor.ActorLocation - DamageOrigin).GetSafeNormal().VectorPlaneProject(FVector::UpVector);

		if(TakeDamageComp != nullptr && !FMath::IsNearlyZero(Damage))
		{
			TakeDamageComp.TakeDamage(Owner, Owner, DamageOrigin, Damage);

			if(DealDamage.TextData != nullptr)
			{
				Gameplay::CreateAnimatedText(TargetActor.ActorLocation, "" + int(Damage), DealDamage.TextData, TargetActor);
			}
		}

		UKnockbackComponent KnockbackComp = UKnockbackComponent::Get(TargetActor);

		if(KnockbackComp != nullptr && !FMath::IsNearlyZero(Damage) && !bKillshot)
		{
			KnockbackComp.bKnockback = true;
			KnockbackComp.Direction = DirectionToDamage;
		}

	}

	UFUNCTION(BlueprintOverride)
	ECapabilityDeactivation ShouldDeactivate() const
	{
		return ECapabilityDeactivation::Local;
	}
}
