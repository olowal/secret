#include "SecTestFindComponentsComponent.h"
#include "GameFramework/Actor.h"

void USecTestFindComponentsComponent::BeginPlay()
{
	Super::BeginPlay();

	if(bRunOnBeginPlay)
		TestFindComponents();
}

void USecTestFindComponentsComponent::TestFindComponents() const
{
	TArray<UActorComponent*> AllComponents;
	const AActor* OwnerActor = GetOwner();
	OwnerActor->GetComponents(AllComponents);

	TArray<TSubclassOf<UActorComponent>> AllClassTypes;

	for (UActorComponent* Comp : AllComponents)
	{
		if (Comp == nullptr)
			continue;

		UClass* ComponentClass = Comp->GetClass();
		if (ComponentClass != nullptr && !AllClassTypes.Contains(Comp->GetClass()))
		{
			AllClassTypes.Add(ComponentClass);
		}
	}

	int32 RandMax = AllClassTypes.Num() - 1;

	if (RandMax == 0)
		return;

	double CycleCount = 0.0;
	{
		SCOPE_SECONDS_COUNTER(CycleCount);

		for (int32 Count = 0; Count < NumTries; ++Count)
		{
			const int32 RandClassIndex = FMath::RandRange(0, RandMax);
			TSubclassOf<UActorComponent> RandomClassType = AllClassTypes[RandClassIndex];
			UActorComponent* SomeComponent = OwnerActor->GetComponentByClass(RandomClassType);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("TestFindCOmponents %.5f"), CycleCount);
}
