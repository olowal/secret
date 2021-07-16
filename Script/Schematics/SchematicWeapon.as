
class UWeaponAttackInfo_Deprecated : UDataAsset
{
    UPROPERTY()
    UAnimMontage AttackAnimation;

    UPROPERTY()
    TSubclassOf<UDamageType> DamageType = UDamageType::StaticClass();

    UPROPERTY()
    float StartDelay = 0.1f;

    UPROPERTY()
    float AttackDuration = 0.5f;
}

class USchematicWeapon : UDataAsset
{
    UPROPERTY()
    TSubclassOf<ASecWeaponBase> WeaponActor;

    UPROPERTY()
    float DamageBase = 10.0f;

    UPROPERTY()
    TArray<UWeaponAttackInfo_Deprecated> AttackInfo;
}
