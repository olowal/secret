#include "SecPriorityList.h"

void FSecInstigatorObjectList::AddInstigator(FName InstigatorToAdd)
{
	ensureMsgf(!InstigatorList.Contains(InstigatorToAdd), TEXT("Instigator %s already exists."), *InstigatorToAdd.ToString());
	InstigatorList.AddUnique(InstigatorToAdd);
}

void FSecPriorityListContainer::RemoveByInstigator(UObject* InInstigator)
{
	if (!ensure(InInstigator != nullptr))
		return;

	const FName InstigatorName = InInstigator->GetFName();

	for (int32 Index = InstigatorList.Num() - 1; Index >= 0; --Index)
	{
		InstigatorList[Index].RemoveInstigator(InstigatorName);

		if (InstigatorList[Index].Num() == 0)
		{
			InstigatorList.RemoveAt(Index);
		}
	}
}

void FSecPriorityListContainer::RemoveByObject(UObject* InObject)
{
	for (int32 Index = InstigatorList.Num() - 1; Index >= 0; --Index)
	{
		if (InstigatorList[Index].ObjectType == InObject)
		{
			InstigatorList.RemoveAt(Index);
		}
	}
}

void FSecPriorityListContainer::Add(UObject* InObject, UObject* InInstigator)
{
	if (!ensure(InInstigator != nullptr))
		return;

	const FName InstigatorName = InInstigator->GetFName();

	Add(InObject, InstigatorName);
}

void FSecPriorityListContainer::Add(UObject* InObject, FName InInstigatorName)
{
	int32 Index = InstigatorList.Find(InObject);

	if (Index != INDEX_NONE)
	{
		FSecInstigatorObjectList& ObjectList = InstigatorList[Index];
		ObjectList.AddInstigator(InInstigatorName);
	}
	else
	{
		InstigatorList.Add(FSecInstigatorObjectList(InObject, InInstigatorName));
	}
}

void FSecPriorityList::Add(UObject* InObject, UObject* InInstigator, int32 InPriority)
{
	if (!ensure(InInstigator != nullptr))
		return;

	const FName InstigatorName = InInstigator->GetFName();

	int32 PriorityIndex = PriorityList.Find(InPriority);

	if (PriorityIndex != INDEX_NONE)
	{
		FSecPriorityListContainer& ListContainer = PriorityList[PriorityIndex];
		int32 InstigatorIndex = ListContainer.InstigatorList.Find(InObject);
		if (InstigatorIndex != INDEX_NONE)
		{
			FSecInstigatorObjectList& InstigatorObjectList = ListContainer.InstigatorList[InstigatorIndex];
			InstigatorObjectList.AddInstigator(InstigatorName);
		}
		else
		{
			ListContainer.InstigatorList.Add(FSecInstigatorObjectList(InObject, InstigatorName));
		}
	}
	else
	{
		Internal_Add(InObject, InstigatorName, InPriority);
	}
}

void FSecPriorityList::RemoveByInstigator(UObject* InInstigator)
{
	for (int32 Index = PriorityList.Num() - 1; Index >= 0; --Index)
	{
		FSecPriorityListContainer& ListContainer = PriorityList[Index];
		ListContainer.RemoveByInstigator(InInstigator);

		if (ListContainer.InstigatorList.Num() == 0)
		{
			PriorityList.RemoveAt(Index);
		}
	}
}

void FSecPriorityList::RemoveByObject(UObject* InObject)
{
	for (int32 Index = PriorityList.Num() - 1; Index >= 0; --Index)
	{
		FSecPriorityListContainer& ListContainer = PriorityList[Index];
		ListContainer.RemoveByObject(InObject);

		if (ListContainer.InstigatorList.Num() == 0)
		{
			PriorityList.RemoveAt(Index);
		}
	}
}

void FSecPriorityList::Internal_Add(UObject* InObject, FName InInstigatorName, int32 InPriority)
{
	FSecPriorityListContainer ListContainer;
	ListContainer.Priority = InPriority;
	ListContainer.InstigatorList.Add(FSecInstigatorObjectList(InObject, InInstigatorName));
	PriorityList.Add(ListContainer);
	PriorityList.Sort();
}
