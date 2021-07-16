import Equipment.ShieldComponent;

class UEquipShieldCapability : USecCapability
{
	UShieldComponent ShieldComp;

	UPROPERTY(Replicated)
	UShieldSettings ServerShieldSettings = nullptr;

	UShieldSettings CurrentShieldSettings = nullptr;

	UFUNCTION(BlueprintOverride)
	void Setup()
	{
		ShieldComp = UShieldComponent::Get(Owner);
	}

    UFUNCTION(BlueprintOverride)
    ECapabilityActivation ShouldActivate() const
    {
		const bool bIsLocallyControlled = IsLocallyControlled();

		if(bIsLocallyControlled && ShieldComp.ShieldSettings != CurrentShieldSettings)
		{
			return ECapabilityActivation::Server;
		}

		if(!bIsLocallyControlled && ServerShieldSettings != CurrentShieldSettings)
		{
			return ECapabilityActivation::Local;
		}

		return ECapabilityActivation::DontActivate;
	}

    UFUNCTION(BlueprintOverride)
    void PreActivation(FActivationSyncParams& SyncParams)
    {
		SyncParams.AddObject(n"ShieldSettings", ShieldComp.ShieldSettings);
    }

	UFUNCTION(BlueprintOverride)
	EServerCapabilityActivation Server_ConfirmActivation(FCapabilityActivationParams ActivationParams, FActivationSyncParams& SyncParams)
	{
		ServerShieldSettings = Cast<UShieldSettings>(ActivationParams.GetObject(n"ShieldSettings"));
		return EServerCapabilityActivation::Activate;
	}

	UFUNCTION(BlueprintOverride)
	void OnActivate(FCapabilityActivationParams ActivationParams)
	{
		if(IsLocallyControlled())
		{
			EquipShield(ShieldComp.ShieldSettings);
		}
		else
		{
			EquipShield(ServerShieldSettings);
		}
	}

	private void EquipShield(UShieldSettings NewShieldSettings)
	{
		if(NewShieldSettings == nullptr)
		{
			ShieldComp.SetSkeletalMesh(nullptr);
			ShieldComp.bIsEquipped = false;
			CurrentShieldSettings = nullptr;
		}
		else
		{
			ShieldComp.SetSkeletalMesh(NewShieldSettings.SkeletalMesh);
			CurrentShieldSettings = NewShieldSettings;
			ShieldComp.bIsEquipped = true;
		}
	}

    UFUNCTION(BlueprintOverride)
    ECapabilityDeactivation ShouldDeactivate() const
    {
		return ECapabilityDeactivation::Local;
	}
}
