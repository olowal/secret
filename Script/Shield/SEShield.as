
class ASEShield : ASecShieldBase
{
    UPROPERTY(DefaultComponent, RootComponent)
    USceneComponent RootComp;

    UPROPERTY(DefaultComponent, Attach = RootComp)
    UStaticMeshComponent Mesh;
    default Mesh.SetCollisionProfileName(n"NoCollision");
}
