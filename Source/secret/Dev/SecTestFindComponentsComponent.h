#pragma once

#include "Components/ActorComponent.h"
#include "SecTestFindComponentsComponent.generated.h"

UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class USecTestFindComponentsComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Test")
	void TestFindComponents() const;

	UPROPERTY(EditAnywhere, Category = "Test")
	int32 NumTries = 10000;

	UPROPERTY(EditAnywhere, Category = "Test")
	bool bRunOnBeginPlay = false;
};

DECLARE_STATS_GROUP(TEXT("Component"), STATGROUP_Component, STATCAT_Advanced);
