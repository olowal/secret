import Equipment.Settings.ShieldSettings;

class UShieldComponent : USkeletalMeshComponent
{
	UPROPERTY()
	UShieldSettings ShieldSettings;

	bool bIsEquipped = false;
	bool bIsBlocking = false;
}

UFUNCTION()
void EquipShield(AActor TargetActor, UShieldSettings Shield)
{
	if(!ensure(TargetActor != nullptr))
		return;
	if(!ensure(Shield != nullptr))
		return;
	UShieldComponent ShieldComp = UShieldComponent::GetOrCreate(TargetActor);
	ShieldComp.ShieldSettings = Shield;
}
