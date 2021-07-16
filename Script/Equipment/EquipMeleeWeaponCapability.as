import Equipment.MeleeWeaponComponent;

class UEquipMeleeWeaponCapability : USecCapability
{
	UMeleeWeaponComponent WeaponComp;

	UMeleeWeaponSettings CurrentWeaponSettings = nullptr;

	UPROPERTY(Replicated)
	UMeleeWeaponSettings ServerWeaponSettings = nullptr;

	UFUNCTION(BlueprintOverride)
	void Setup()
	{
		WeaponComp = UMeleeWeaponComponent::Get(Owner);
	}

    UFUNCTION(BlueprintOverride)
    ECapabilityActivation ShouldActivate() const
    {
		const bool bIsLocallyControlled = IsLocallyControlled();

		if(bIsLocallyControlled && WeaponComp.WeaponSettings != CurrentWeaponSettings)
		{
			return ECapabilityActivation::Server;
		}

		if(!bIsLocallyControlled && ServerWeaponSettings != CurrentWeaponSettings)
		{
			return ECapabilityActivation::Local;
		}

		return ECapabilityActivation::DontActivate;
	}

    UFUNCTION(BlueprintOverride)
    void PreActivation(FActivationSyncParams& SyncParams)
    {
        SyncParams.AddObject(n"WeaponSettings", WeaponComp.WeaponSettings);
    }

	UFUNCTION(BlueprintOverride)
	EServerCapabilityActivation Server_ConfirmActivation(FCapabilityActivationParams ActivationParams, FActivationSyncParams& SyncParams)
	{
		ServerWeaponSettings = Cast<UMeleeWeaponSettings>(ActivationParams.GetObject(n"WeaponSettings"));
		return EServerCapabilityActivation::Activate;
	}

	UFUNCTION(BlueprintOverride)
	void OnActivate(FCapabilityActivationParams ActivationParams)
	{
		if(IsLocallyControlled())
		{
			EquipMeleeWeapon(WeaponComp.WeaponSettings);
		}
		else
		{
			EquipMeleeWeapon(ServerWeaponSettings);
		}
		
	}

	private void EquipMeleeWeapon(UMeleeWeaponSettings NewWeaponSettings)
	{
		if(NewWeaponSettings == nullptr)
		{
			WeaponComp.SetSkeletalMesh(nullptr);
			WeaponComp.bIsEquipped = false;
		}
		else
		{
			WeaponComp.SetSkeletalMesh(NewWeaponSettings.SkeletalMesh);
			WeaponComp.bIsEquipped = true;
		}

		CurrentWeaponSettings = NewWeaponSettings;
	}

    UFUNCTION(BlueprintOverride)
    ECapabilityDeactivation ShouldDeactivate() const
    {
		return ECapabilityDeactivation::Local;
	}
}
