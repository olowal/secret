import Equipment.Settings.MeleeWeaponSettings;

enum EMeleeWeaponAttackState
{
	None,
	Hold,
	Release
}

struct FMeleeWeaponAttackInfo
{
	UAnimMontage AttackAnim = nullptr;
}

class UMeleeWeaponComponent : USkeletalMeshComponent
{
	// This is the weapon we want to equip
	UPROPERTY()
	UMeleeWeaponSettings WeaponSettings;

	UAnimMontage LastPlayedMontage;
	UAnimMontage AttackMontageCurrent;

	// Count the number of times AttackHold has been activated.
	int NumHold = 0;
	// Count the number of times AttackRelease has been activated.
	int NumRelease = 0;

	private int AttackIndex = -1;

	bool bWantsToAttack = false;
	bool bHoldDone = false;

	EMeleeWeaponAttackState AttackState = EMeleeWeaponAttackState::None;

	int GetIndexCurrent() const { return AttackIndex; }
	int GetIndexTotal() const
	{
		if(WeaponSettings == nullptr)
			return 0;

		return WeaponSettings.AttackInfo.Num();
	}

	void ResetAttackIndex()
	{
		AttackIndex = 0;
	}

	void IncrementAttackIndex()
	{
		if(WeaponSettings == nullptr)
			return;

		AttackIndex = SecMath::IncrementIndex(AttackIndex, WeaponSettings.AttackInfo.Num());
	}

	bool bIsEquipped = false;

	UMeleeWeaponAttackSettings GetAttackSettingsCurrent() const property
	{
		if(!ensure(WeaponSettings != nullptr))
			return nullptr;

		if(!ensure(AttackIndex < WeaponSettings.AttackInfo.Num()))
			return nullptr;

		if(!ensure(AttackIndex > -1))
			return nullptr;

		return WeaponSettings.AttackInfo[AttackIndex];
	}

	UAnimMontage GetMontageCurrent() const
	{
		const UMeleeWeaponAttackSettings AttackSettings = GetAttackSettingsCurrent();
		if(AttackSettings == nullptr)
			return nullptr;

		return AttackSettingsCurrent.AttackAnimation;
	}

	float GetStartDelayCurrent() const
	{
		const UMeleeWeaponAttackSettings AttackSettings = GetAttackSettingsCurrent();
		if(AttackSettings == nullptr)
			return 0.0f;
		
		return AttackSettingsCurrent.StartDelay;
	}

	float GetAttackDurationCurrent() const
	{
		const UMeleeWeaponAttackSettings AttackSettings = GetAttackSettingsCurrent();
		if(AttackSettings == nullptr)
			return 0.0f;

		return AttackSettingsCurrent.AttackDuration;
	}
}

UFUNCTION()
void EquipMeleeWeapon(AActor TargetActor, UMeleeWeaponSettings MeleeWeapon)
{
	if(!ensure(TargetActor != nullptr))
		return;
	if(!ensure(MeleeWeapon != nullptr))
		return;
	UMeleeWeaponComponent WeaponComp = UMeleeWeaponComponent::GetOrCreate(TargetActor);
	WeaponComp.WeaponSettings = MeleeWeapon;
}
