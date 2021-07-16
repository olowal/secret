import Equipment.MeleeWeaponComponent;

class UBTTask_AttackHold : UBTTask_BlueprintBase
{
	int LastNumRelease = 0;
	int ElapsedFrames = 0;

	USecAttributeComponent Attribute = nullptr;
	UMeleeWeaponComponent WeaponComp = nullptr;

	UFUNCTION(BlueprintOverride)
	void ExecuteAI(AAIController AIController, APawn ControlledPawn)
	{
		Attribute = USecAttributeComponent::Get(ControlledPawn);
		WeaponComp = UMeleeWeaponComponent::Get(ControlledPawn);

		if(Attribute == nullptr)
		{
			FinishExecute(false);
		}

		if(WeaponComp == nullptr)
		{
			FinishExecute(false);
		}

		ElapsedFrames = 0;
		LastNumRelease = WeaponComp.NumRelease;
		WeaponComp.bWantsToAttack = true;
		WeaponComp.IncrementAttackIndex();
	}

	UFUNCTION(BlueprintOverride)
	void TickAI(AAIController OwnerController, APawn ControlledPawn, float DeltaTime)
	{
		ElapsedFrames++;

		if(ElapsedFrames > 10)
		{
			WeaponComp.bWantsToAttack = false;
			FinishExecute(false);
		}

		if(WeaponComp.NumRelease > LastNumRelease)
		{
			WeaponComp.bWantsToAttack = false;
			FinishExecute(true);
		}
	}

	UFUNCTION(BlueprintOverride)
	void AbortAI(AAIController OwnerController, APawn ControlledPawn)
	{
		
	}
}
