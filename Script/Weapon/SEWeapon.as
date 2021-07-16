
UCLASS(abstract)
class ASEWeapon : ASecWeaponBase
{
    UPROPERTY(DefaultComponent, RootComponent)
    USceneComponent Root;

    UPROPERTY(DefaultComponent, Attach = Root)
    USkeletalMeshComponent WeaponMesh;
    default WeaponMesh.SetCollisionProfileName(n"NoCollision");
}
