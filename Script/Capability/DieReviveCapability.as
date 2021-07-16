import Components.TakeDamageComponent;
import Attribute.HealthMaximumAttribute;
import Equipment.MeleeWeaponComponent;
import Equipment.ShieldComponent;

class UDieReviveCapability : USecCapability
{
	UTakeDamageComponent TakeDamageComp;
	UCharacterMovementComponent MoveComp;
	UMeleeWeaponComponent MeleeWeaponComp;
	UShieldComponent ShieldComp;
	ASecCharacter Character;

	FVector RelativeLocation;
	FRotator RelativeRotation;

	UHealthAttribute Health;
	UHealthMaximumAttribute HealthMaximum;

	float Elapsed = 0;
	float TargetElapsed = 3;
	bool bDroppedWeapons = false;

	UFUNCTION(BlueprintOverride)
	void Setup()
	{
		TakeDamageComp = UTakeDamageComponent::Get(Owner);
		Character = Cast<ASecCharacter>(Owner);
		MoveComp = UCharacterMovementComponent::Get(Owner);
		ShieldComp = UShieldComponent::Get(Owner);
		MeleeWeaponComp = UMeleeWeaponComponent::Get(Owner);

		Health = Cast<UHealthAttribute>(Attribute.GetAttributeByClass(UHealthAttribute::StaticClass()));
		HealthMaximum = Cast<UHealthMaximumAttribute>(Attribute.GetAttributeByClass(UHealthMaximumAttribute::StaticClass()));
	}

    UFUNCTION(BlueprintOverride)
    ECapabilityActivation ShouldActivate() const
    {
        if(TakeDamageComp.bIsAlive)
			return ECapabilityActivation::DontActivate;

        return ECapabilityActivation::Network;
    }

	UFUNCTION(BlueprintOverride)
	EServerCapabilityActivation Server_ConfirmActivation(FCapabilityActivationParams ActivationParams, FActivationSyncParams& SyncParams)
	{
		SyncParams.AddVector(n"LastDamageOrigin", TakeDamageComp.LastDamageLocation);
		return EServerCapabilityActivation::Activate;
	}

	UFUNCTION(BlueprintOverride)
	void OnActivate(FCapabilityActivationParams ActivationParams)
	{
		bDroppedWeapons = false;
		Elapsed = TargetElapsed;
		BlockCapabilities(CapabilityTags::Input, this);
		Character.CapsuleComponent.SetCollisionProfileName(n"NoCollision");
		RelativeLocation = Character.Mesh.RelativeLocation;
		RelativeRotation = Character.Mesh.RelativeRotation;
		Character.SetReplicateMovement(false);
		Character.Mesh.SetCollisionProfileName(n"PhysicsActor");
		Character.Mesh.SetSimulatePhysics(true);
		FVector LastDamageOrigin = ActivationParams.GetVector(n"LastDamageOrigin");
		const FVector Impulse = (Owner.ActorLocation - LastDamageOrigin).GetSafeNormal() * 8000;
		Character.Mesh.AddImpulse(Impulse, n"spine_03", true);
	}

	UFUNCTION(BlueprintOverride)
	void TickActive(float DeltaTime)
	{
		Elapsed -= DeltaTime;

		if(!bDroppedWeapons && Elapsed < (TargetElapsed * 0.92f))
		{
			bDroppedWeapons = true;

			FVector WeaponLinearVel = Character.Mesh.GetPhysicsLinearVelocity(n"hand_r");
			FVector ShieldLinearVel = Character.Mesh.GetPhysicsLinearVelocity(n"hand_l");
			//MeleeWeaponComp.DetachFromComponent(EDetachmentRule::KeepWorld);
			//ShieldComp.DetachFromComponent(EDetachmentRule::KeepWorld);
			
			MeleeWeaponComp.SetCollisionProfileName(n"PhysicsActor");
			MeleeWeaponComp.SetSimulatePhysics(true);
			ShieldComp.SetCollisionProfileName(n"PhysicsActor");
			ShieldComp.SetSimulatePhysics(true);
			ShieldComp.SetPhysicsLinearVelocity(ShieldLinearVel, true);
			MeleeWeaponComp.SetPhysicsLinearVelocity(WeaponLinearVel, true);
		}
	}

    UFUNCTION(BlueprintOverride)
    ECapabilityDeactivation ShouldDeactivate() const
    {
        if(Elapsed <= 0)
			return ECapabilityDeactivation::Network;

		return ECapabilityDeactivation::DontDeactivate;
    }

    UFUNCTION(BlueprintOverride)
    void OnDeactivate(FCapabilityDeactivationParams& DeactivationParams)
    {
		UnblockCapabilities(CapabilityTags::Input, this);
		Character.Mesh.SetSimulatePhysics(false);
		Character.CapsuleComponent.SetCollisionProfileName(n"Character");
		Character.Mesh.SetCollisionProfileName(n"CharacterMesh");
		Character.Mesh.AttachToComponent(Character.CapsuleComponent);		
		Character.Mesh.SetRelativeLocationAndRotation(RelativeLocation, RelativeRotation);
		
		Health.ModifyBaseValue(8);
		Character.SetReplicateMovement(true);

		MeleeWeaponComp.SetSimulatePhysics(false);
		ShieldComp.SetSimulatePhysics(false);
		MeleeWeaponComp.SetCollisionProfileName(n"NoCollision");
		ShieldComp.SetCollisionProfileName(n"NoCollision");
		MeleeWeaponComp.AttachToComponent(Character.Mesh, n"Weapon", EAttachmentRule::SnapToTarget);
		ShieldComp.AttachToComponent(Character.Mesh, n"Shield", EAttachmentRule::SnapToTarget);
		Character.StopAnimMontage();

    }
}

