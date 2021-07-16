
class UMeleeWeaponAttackSettings : UDataAsset
{
    UPROPERTY()
    UAnimMontage AttackAnimation;

    UPROPERTY()
    float StartDelay = 0.1f;

    UPROPERTY()
    float AttackDuration = 0.5f;
}


class UMeleeWeaponSettings : UDataAsset
{
	UPROPERTY()
	TArray<UMeleeWeaponAttackSettings> AttackInfo;

    UPROPERTY()
    UAnimMontage AttackAnim;

	UPROPERTY(Category = Mesh)
	USkeletalMesh SkeletalMesh;
}
