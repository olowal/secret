#pragma once

#include "BehaviorTree/BTDecorator.h"
#include "AI/BehaviorTree/SecBehaviorTreeStatics.h"
#include "SecBTDecorator_DistanceToEntry.generated.h"

class UBehaviorTree;
class UBlackboardComponent;
class USecAttribute;
class APawn;

/*
	Compare the distance (squared) between two entires.
*/

UCLASS(HideCategories = (Condition))
class SECRET_API USecBTDecorator_DistanceToEntry : public UBTDecorator
{
	GENERATED_BODY()
public:
	struct FSecBTDecoratorMemory
	{
		USecAttribute* DistanceAttribute = nullptr;
	};

	USecBTDecorator_DistanceToEntry(const FObjectInitializer& ObjectInitializer);

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	virtual FString GetStaticDescription() const override;

	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;

	virtual uint16 GetInstanceMemorySize() const override;

	float GetDistance(const UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const;

	// Get location from selected entry. bSuccess will be set to false if entry is Blackboard and Object, but currently nullptr.
	FVector GetFrom(const UBehaviorTreeComponent& OwnerComp, bool& bSuccess) const;
	// Get location from selected entry. bSuccess will be set to false if entry is Blackboard and Object, but currently nullptr.
	FVector GetTo(const UBehaviorTreeComponent& OwnerComp, bool& bSuccess) const;

	const APawn* GetPawnFromBehaviorTree(const UBehaviorTreeComponent& OwnerComp) const;

	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, Category = Entry)
	ESecBTDistanceComparisonEntry ComparisonType = ESecBTDistanceComparisonEntry::Greater;

	UPROPERTY(EditAnywhere, Category = Entry)
	ESecBTEntryType DistanceEntryType = ESecBTEntryType::Value;

	UPROPERTY(EditAnywhere, Category = Entry, meta = (EditCondition = "DistanceEntryType == ESecBTEntryType::Value", EditConditionHides, DisplayName = "Distance"))
	float DistanceValue = 200.0f;
	UPROPERTY(EditAnywhere, Category = Entry, meta = (EditCondition = "DistanceEntryType == ESecBTEntryType::BBEntry", EditConditionHides, DisplayName = "Distance"))
	FBlackboardKeySelector DistanceBlackboard;
	UPROPERTY(EditAnywhere, Category = Entry, meta = (EditCondition = "DistanceEntryType == ESecBTEntryType::Attribute", EditConditionHides, DisplayName = "Distance"))
	TSubclassOf<USecAttribute> DistanceAttribute;


	UPROPERTY(EditAnywhere, Category = Entry)
	ESecBTEntryType FromEntryType = ESecBTEntryType::Self;

	UPROPERTY(EditAnywhere, Category = Entry, meta = (EditCondition = "FromEntryType == ESecBTEntryType::BBEntry", EditConditionHides, DisplayName = "From"))
	FBlackboardKeySelector FromBlackboard;

	UPROPERTY(EditAnywhere, Category = Entry)
	ESecBTEntryType ToEntryType = ESecBTEntryType::BBEntry;

	UPROPERTY(EditAnywhere, Category = Entry, meta = (EditCondition = "ToEntryType == ESecBTEntryType::BBEntry", EditConditionHides, DisplayName = "To"))
	FBlackboardKeySelector ToBlackboard;

	// Select to use DistSquared2D instead of DistSquared.
	UPROPERTY(EditAnywhere, Category = Entry)
	bool bDistSquared2D = true;

};
