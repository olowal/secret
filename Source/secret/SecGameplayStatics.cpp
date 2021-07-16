#include "SecGameplayStatics.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameState/SecGameStateBase.h"
#include "Camera/SecThirdPersonCameraBase.h"
#include "GameFramework/Actor.h"
#include "GameFramework/DamageType.h"
#include "Capability/SecCapabilitySystemComponent.h"

class ASecThirdPersonCameraBase* USecGameplayStatics::GetThirdPersonCamera(const UObject* WorldContextObject)
{
	if (UWorld * World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		if (ASecGameStateBase* GameState = Cast<ASecGameStateBase>(World->GetGameState()))
		{
			return GameState->ThirdPersonCamera;
		}
	}

	return nullptr;
}

void USecGameplayStatics::DealDamageToActor(AActor* DamagedActor, float BaseDamage, AController* EventInstigator, AActor* DamageCauser, TSubclassOf<class UDamageType> DamageTypeClass)
{
	if (DamagedActor && (BaseDamage != 0.f))
	{
		// make sure we have a good damage type
		TSubclassOf<UDamageType> const ValidDamageTypeClass = DamageTypeClass ? DamageTypeClass : TSubclassOf<UDamageType>(UDamageType::StaticClass());
		FDamageEvent DamageEvent(ValidDamageTypeClass);

		DamagedActor->TakeDamage(BaseDamage, DamageEvent, EventInstigator, DamageCauser);
	}
}

void USecGameplayStatics::AddCapabilitySheetToActor(const UObject* WorldContextObject, AActor* TargetActor, USecCapabilitySheet* Sheet, ESecScriptPriority Priority)
{
	if (!HasAuthority(WorldContextObject))
	{
		// Log message because failing on authority.
		return;
	}

	if (!ensureMsgf(TargetActor != nullptr, TEXT("Invalid TargetActor passed as parameter")))
		return;

	USecCapabilitySystemComponent* CapabilitySystemComponent = TargetActor->FindComponentByClass<USecCapabilitySystemComponent>();

	if (!CapabilitySystemComponent)
	{
		CapabilitySystemComponent = NewObject<USecCapabilitySystemComponent>(TargetActor, USecCapabilitySystemComponent::StaticClass());
		if (!ensureMsgf(CapabilitySystemComponent != nullptr, TEXT("unable to instantiate CapabilitySystemComponent for %s."), *TargetActor->GetName()))
			return;
		CapabilitySystemComponent->RegisterComponent();
	}

	CapabilitySystemComponent->ApplySheet(Sheet, Priority);
}

bool USecGameplayStatics::HasAuthority(const UObject* WorldContextObject)
{
	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull))
	{
		return World->IsServer();
	}

	return false;
}
