#include "SecBTDecorator_DistanceToEntry.h"
#include "SecUtilityStatics.h"
#include "Attribute/SecAttribute.h"
#include "Attribute/SecAttributeComponent.h"
#include "GameFramework/Pawn.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Float.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"

typedef USecBTDecorator_DistanceToEntry::FSecBTDecoratorMemory DecoratorMemory;

USecBTDecorator_DistanceToEntry::USecBTDecorator_DistanceToEntry(const FObjectInitializer& ObjectInitializer)
{
	NodeName = TEXT("Distance comparison");

	bNotifyBecomeRelevant = true;
}

bool USecBTDecorator_DistanceToEntry::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const float Distance = GetDistance(OwnerComp, NodeMemory);

	bool bFromSuccess = false;
	bool bToSuccess = false;

	const FVector From = GetFrom(OwnerComp, bFromSuccess);
	const FVector To = GetTo(OwnerComp, bToSuccess);

	if (!bFromSuccess || !bToSuccess)
		return false;

	const float DistSquared = bDistSquared2D ? FVector::DistSquared2D(From, To) : FVector::DistSquared(From, To);

	if (ComparisonType == ESecBTDistanceComparisonEntry::Greater &&
		DistSquared > FMath::Square(Distance))
	{
		return true;
	}
	else if (ComparisonType == ESecBTDistanceComparisonEntry::Lower &&
		DistSquared < FMath::Square(Distance))
	{
		return true;
	}
	
	return false;
}

FString USecBTDecorator_DistanceToEntry::GetStaticDescription() const
{
	FString FromString, ToString, WarningString;

	if (FromEntryType == ESecBTEntryType::Self)
		FromString = TEXT("Self");
	else if (FromEntryType == ESecBTEntryType::BBEntry)
	{
		FromString = FromBlackboard.SelectedKeyName.ToString();
		if (FromBlackboard.SelectedKeyType != UBlackboardKeyType_Vector::StaticClass() && 
			FromBlackboard.SelectedKeyType != UBlackboardKeyType_Object::StaticClass())
		{
			WarningString += TEXT("\nWarning: Invalid FromEntryType BBEntry. Must be Vector or Object");
		}
	}
	else
	{
		FromString = TEXT("INVALID");
		WarningString += TEXT("\nWarning: Invalid FromEntryType. Must be Self or BBEntry");
	}
		

	if (ToEntryType == ESecBTEntryType::Self)
		ToString = TEXT("Self");
	else if (ToEntryType == ESecBTEntryType::BBEntry)
	{
		ToString = ToBlackboard.SelectedKeyName.ToString();
		if (ToBlackboard.SelectedKeyType != UBlackboardKeyType_Vector::StaticClass() &&
			ToBlackboard.SelectedKeyType != UBlackboardKeyType_Object::StaticClass())
		{
			WarningString += TEXT("\nWarning: Invalid ToEntryType BBEntry. Must be Vector or Object");
		}
	}
	else
	{
		ToString = TEXT("INVALID");
		WarningString += TEXT("\nWarning: Invalid ToEntryType. Must be Self or BBEntry");
	}
	
	const FString StartDesc = 
		FString::Printf(TEXT("Distance between:\n%s and %s is %s ")
			, *FromString
			, *ToString
			, *USecUtilityStatics::EnumToString(ComparisonType));

	FString EndDesc;
	if (DistanceEntryType == ESecBTEntryType::Value)
	{
		EndDesc = FString::Printf(TEXT("than %.3f."), DistanceValue);
	}
	else if (DistanceEntryType == ESecBTEntryType::Attribute)
	{
		if(DistanceAttribute != nullptr)
			EndDesc = FString::Printf(TEXT("than the value of %s"), *DistanceAttribute->GetDefaultObject()->GetName());
		else
			EndDesc = FString::Printf(TEXT("null Attribute"));
	}
	else if (DistanceEntryType == ESecBTEntryType::BBEntry)
	{
		EndDesc = FString::Printf(TEXT("than the value of %s"), *DistanceBlackboard.SelectedKeyName.ToString());
		if (DistanceBlackboard.SelectedKeyType != UBlackboardKeyType_Float::StaticClass())
		{
			WarningString += FString::Printf(TEXT("\nWARNING: Distance BBEntry is not a float."));
		}
	}

	const FString FullDescription = StartDesc + EndDesc + WarningString;
	return FullDescription;
}

void USecBTDecorator_DistanceToEntry::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	UBlackboardData* BBAsset = GetBlackboardAsset();
	if (ensure(BBAsset))
	{
		if(DistanceEntryType == ESecBTEntryType::BBEntry)
			DistanceBlackboard.ResolveSelectedKey(*BBAsset);

		if (FromEntryType == ESecBTEntryType::BBEntry)
			FromBlackboard.ResolveSelectedKey(*BBAsset);

		if(ToEntryType == ESecBTEntryType::BBEntry)
			ToBlackboard.ResolveSelectedKey(*BBAsset);
	}
}

uint16 USecBTDecorator_DistanceToEntry::GetInstanceMemorySize() const
{
	return sizeof(DecoratorMemory);
}

float USecBTDecorator_DistanceToEntry::GetDistance(const UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	if (DistanceEntryType == ESecBTEntryType::Value)
	{
		return DistanceValue;
	}
	else if (DistanceEntryType == ESecBTEntryType::BBEntry)
	{
		const UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

		if (BlackboardComp && DistanceBlackboard.SelectedKeyType == UBlackboardKeyType_Float::StaticClass())
		{
			return BlackboardComp->GetValue<UBlackboardKeyType_Float>(DistanceBlackboard.GetSelectedKeyID());
		}
	}
	else if(DistanceEntryType == ESecBTEntryType::Attribute)
	{
		DecoratorMemory* Memory = (DecoratorMemory*)NodeMemory;
		if (!ensure(Memory != nullptr))
			return 0.0f;
		
		if (Memory->DistanceAttribute != nullptr)
		{
			return Memory->DistanceAttribute->GetAttributeValue();
		}
	}

	return 0.0f;
}

FVector USecBTDecorator_DistanceToEntry::GetFrom(const UBehaviorTreeComponent& OwnerComp, bool& bSuccess) const
{
	bSuccess = true;

	if (FromEntryType == ESecBTEntryType::Self)
	{
		const APawn* ControlledPawn = GetPawnFromBehaviorTree(OwnerComp);
#if !UE_BUILD_SHIPPING
		if (!ensure(ControlledPawn != nullptr))
			return FVector::ZeroVector;
#endif // !UE_BUILD_SHIPPING
		return ControlledPawn->GetActorLocation();
	}
	else if (FromEntryType == ESecBTEntryType::BBEntry)
	{
		const UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

		if (BlackboardComp && FromBlackboard.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
		{
			return BlackboardComp->GetValue<UBlackboardKeyType_Vector>(FromBlackboard.GetSelectedKeyID());
		}
		else if (BlackboardComp && FromBlackboard.SelectedKeyType == UBlackboardKeyType_Object::StaticClass())
		{
			UObject* FromObject = BlackboardComp->GetValue<UBlackboardKeyType_Object>(FromBlackboard.GetSelectedKeyID());
			AActor* FromActor = Cast<AActor>(FromObject);
			if (FromActor != nullptr)
				return FromActor->GetActorLocation();
			else
				bSuccess = false;
		}
	}

	return FVector::ZeroVector;
}

FVector USecBTDecorator_DistanceToEntry::GetTo(const UBehaviorTreeComponent& OwnerComp, bool& bSuccess) const
{
	bSuccess = true;

	if (ToEntryType == ESecBTEntryType::Self)
	{
		const APawn* ControlledPawn = GetPawnFromBehaviorTree(OwnerComp);
#if !UE_BUILD_SHIPPING
		if (!ensure(ControlledPawn != nullptr))
			return FVector::ZeroVector;
#endif // !UE_BUILD_SHIPPING
		return ControlledPawn->GetActorLocation();
	}
	else if (ToEntryType == ESecBTEntryType::BBEntry)
	{
		const UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

		if (BlackboardComp && ToBlackboard.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
		{
			return BlackboardComp->GetValue<UBlackboardKeyType_Vector>(ToBlackboard.GetSelectedKeyID());
		}
		else if (BlackboardComp && ToBlackboard.SelectedKeyType == UBlackboardKeyType_Object::StaticClass())
		{
			UObject* ToObject = BlackboardComp->GetValue<UBlackboardKeyType_Object>(ToBlackboard.GetSelectedKeyID());
			AActor* ToActor = Cast<AActor>(ToObject);
			if (ToActor != nullptr)
				return ToActor->GetActorLocation();
			else
				bSuccess = false;
		}
	}

	return FVector::ZeroVector;
}

const APawn* USecBTDecorator_DistanceToEntry::GetPawnFromBehaviorTree(const UBehaviorTreeComponent& OwnerComp) const
{
	const AAIController* AIController = OwnerComp.GetAIOwner();

#if !UE_BUILD_SHIPPING
	if (!ensure(AIController != nullptr))
		return nullptr;
#endif // !UE_BUILD_SHIPPING

	const APawn* ControlledPawn = AIController->GetPawn();
	ensure(ControlledPawn != nullptr);
	return ControlledPawn;
}

void USecBTDecorator_DistanceToEntry::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (DistanceEntryType != ESecBTEntryType::Attribute)
		return;

	DecoratorMemory* Memory = (DecoratorMemory*)NodeMemory;
	if (!ensure(Memory != nullptr))
		return;

	if (Memory->DistanceAttribute == nullptr)
	{
		const APawn* ControlledPawn = GetPawnFromBehaviorTree(OwnerComp);
#if !UE_BUILD_SHIPPING
		if (!ensure(ControlledPawn != nullptr))
			return;
#endif // !UE_BUILD_SHIPPING

		USecAttributeComponent* AttributeComponent = ControlledPawn->FindComponentByClass<USecAttributeComponent>();

		if (!ensureMsgf(AttributeComponent != nullptr, TEXT("%s: Pawn %s does not contain an AttributeComponent."), *NodeName, *ControlledPawn->GetName()))
			return;

		Memory->DistanceAttribute = AttributeComponent->GetAttributeByClass(DistanceAttribute);
	}
}
