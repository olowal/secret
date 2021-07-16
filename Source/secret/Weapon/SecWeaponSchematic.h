#pragma once

#include "Engine/DataAsset.h"
#include "SecWeaponSchematic.generated.h"

UCLASS(Blueprintable)
class SECRET_API USecWeaponSchematic : public UDataAsset
{
	GENERATED_BODY()
public:
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<class ASecWeaponBase> WeaponActor;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TArray<class UAnimMontage*> AttackAnimations;

	UFUNCTION(BlueprintPure, Category = "ZC|Weapon")
	FORCEINLINE int32 GetNumAttackAnimations() const { return AttackAnimations.Num(); }
};
