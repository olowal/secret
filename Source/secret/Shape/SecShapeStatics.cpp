#include "SecShapeStatics.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"

UPrimitiveComponent* USecShapeStatics::CreateTriggerShape(AActor* InActor, USceneComponent* AttachTo, const FSecShapeSettings& Settings, const FTransform& Transform, const FName& CollisionProfile)
{
	if (!ensure(InActor != nullptr))
		return nullptr;

	if (Settings.Type == ESecShapeType::None)
		return nullptr;

	if (AttachTo == nullptr)
		AttachTo = InActor->GetRootComponent();

	UPrimitiveComponent* Component = CreateShapeInternal(InActor, Settings, Transform);
	if (ensure(Component))
	{
		// Set up all the right settings on the component
		if (AttachTo != nullptr)
		{
			Component->SetMobility(AttachTo->Mobility);
			FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepRelative, false);
			Component->AttachToComponent(AttachTo, AttachmentRules);
		}

		static FName NAME_OverlapAllCollisionProfile("OverlapAll");
		Component->SetGenerateOverlapEvents(true);

		if (CollisionProfile == NAME_None)
			Component->SetCollisionProfileName(NAME_OverlapAllCollisionProfile);
		else
			Component->SetCollisionProfileName(CollisionProfile);

		Component->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}

	return Component;
}

void USecShapeStatics::SetPrimitiveSize(UPrimitiveComponent* PrimitiveComponent, const FSecShapeSettings& Settings)
{
	if (USphereComponent* SphereComponent = Cast<USphereComponent>(PrimitiveComponent))
	{
		SphereComponent->SetSphereRadius(Settings.SphereRadius);
	}
	else if (UBoxComponent* BoxComponent = Cast<UBoxComponent>(PrimitiveComponent))
	{
		BoxComponent->SetBoxExtent(Settings.BoxExtends);
	}
	else if (UCapsuleComponent* CapsuleComponent = Cast<UCapsuleComponent>(PrimitiveComponent))
	{
		CapsuleComponent->SetCapsuleSize(Settings.CapsuleRadius, Settings.CapsuleHalfHeight);
	}
}

UPrimitiveComponent* USecShapeStatics::CreateShapeInternal(AActor* InActor, const FSecShapeSettings& Settings, const FTransform& Transform)
{
	UPrimitiveComponent* Component = nullptr;

	// Create the appropriate shape
	if (Settings.Type == ESecShapeType::Sphere)
		Component = NewObject<USphereComponent>(InActor, USphereComponent::StaticClass());
	else if (Settings.Type == ESecShapeType::Box)
		Component = NewObject<UBoxComponent>(InActor, UBoxComponent::StaticClass());
	else if (Settings.Type == ESecShapeType::Capsule)
		Component = NewObject<UCapsuleComponent>(InActor, UCapsuleComponent::StaticClass());
	else
		return nullptr;

	SetPrimitiveSize(Component, Settings);

	Component->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Component->SetRelativeTransform(Transform);
	Component->RegisterComponent();

	return Component;
}

