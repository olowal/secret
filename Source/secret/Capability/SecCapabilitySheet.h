#pragma once

#include "Engine/DataAsset.h"
#include "Attribute/SecAttributeStatics.h"
#include "SecCapabilitySheet.generated.h"

class USecCapability;
class USecAttributeSheet;
class UActorComponent;
class AAIController;

UCLASS()
class SECRET_API USecCapabilitySheet : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, Category = Capability)
	TSet<TSubclassOf<USecCapability> > Capabilities;
	UPROPERTY(EditDefaultsOnly, Category = Tags)
	TArray<FName> BlockingTags;
	UPROPERTY(EditDefaultsOnly, Category = Component)
	TSet<TSubclassOf<UActorComponent> > Components;
	UPROPERTY(EditDefaultsOnly, Category = Attribute)
	USecAttributeSheet* AttributeSheet;
	// Optional controller to take over Actor. (Must be a pawn)
	UPROPERTY(EditDefaultsOnly, Category = AIController)
	TSubclassOf<AAIController> AIController;
};
