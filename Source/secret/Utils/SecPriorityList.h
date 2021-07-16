#pragma once

#include "CoreMinimal.h"
#include "Capability/SecScriptPriority.h"
#include "SecPriorityList.generated.h"

USTRUCT()
struct FSecInstigatorObjectList
{
	GENERATED_BODY()
public:
	FSecInstigatorObjectList() {}
	FSecInstigatorObjectList(UObject* InObject) : ObjectType(InObject) {}
	FSecInstigatorObjectList(UObject* InObject, FName InInstigator)
	{
		ObjectType = InObject;
		InstigatorList.Add(InInstigator);
	}

	void RemoveInstigator(FName InstigatorToRemove) { InstigatorList.Remove(InstigatorToRemove); }
	void AddInstigator(FName InstigatorToAdd);
	int32 Num() const { return InstigatorList.Num(); }
	bool Contains(FName InInstigator) const { return InstigatorList.Contains(InInstigator); }

	template<typename T>
	T* GetObject() const
	{
		return CastChecked<T>(ObjectType);
	}
		
	TArray<FName> InstigatorList;
	UPROPERTY(Transient)
	UObject* ObjectType;

	bool operator == (const FSecInstigatorObjectList& LHS) const { return this->ObjectType == LHS.ObjectType; }
};

USTRUCT()
struct FSecPriorityListContainer
{
	GENERATED_BODY()
public:
	FSecPriorityListContainer() {}
	FSecPriorityListContainer(int32 InPriority) : Priority(InPriority) {}

	UPROPERTY(Transient)
	TArray<FSecInstigatorObjectList> InstigatorList;
	int32 Priority = 0;

	bool IsEmpty() const
	{
		return InstigatorList.Num() == 0;
	}

	void RemoveByInstigator(UObject* InInstigator);
	void RemoveByObject(UObject* InObject);

	void Add(UObject* InObject, UObject* InInstigator);
	void Add(UObject* InObject, FName InInstigatorName);

	bool operator == (const FSecPriorityListContainer& LHS) const { return this->Priority == LHS.Priority; }
	bool operator < (const FSecPriorityListContainer& LHS) const { return this->Priority < LHS.Priority; }
	bool operator <= (const FSecPriorityListContainer& LHS) const { return this->Priority <= LHS.Priority; }
	bool operator > (const FSecPriorityListContainer& LHS) const { return this->Priority > LHS.Priority; }
	bool operator >= (const FSecPriorityListContainer& LHS) const { return this->Priority >= LHS.Priority; }
};

USTRUCT()
struct FSecPriorityList
{
	GENERATED_BODY()
public:

	template <typename T>
	void GetObjectsByInstigator(TArray<T*>& OutList, UObject* Instigator)
	{
		if (IsEmpty())
			return;

		if (!ensure(Instigator != nullptr))
			return;

		const FName InstigatorName = Instigator->GetFName();

		for (const FSecPriorityListContainer& Priority : PriorityList)
		{
			for (const FSecInstigatorObjectList& InstigatorObjectList : Priority.InstigatorList)
			{
				if (InstigatorObjectList.Contains(InstigatorName))
				{
					OutList.Add(InstigatorObjectList.GetObject<T>());
				}
			}
		}
	}

	bool IsEmpty() const
	{
		return PriorityList.Num() == 0;
	}

	// Returns the object with the highest priority
	template <typename T>
	T* GetHighestPriorityObject() const
	{
		return CastChecked<T>(PriorityList.Last().InstigatorList.Last().ObjectType);
	}

	// Returns the highest priority object within a category
	template <typename T>
	T* GetHighestFromPriority(int32 InPriority) const
	{
		for (const FSecPriorityListContainer& ListContainer : PriorityList)
		{
			if (ListContainer.Priority == InPriority)
			{
				return CastChecked<T>(ListContainer.InstigatorList().Last().ObjectType);
			}
		}

		return nullptr;
	}

	// Get all objects from the list of highest priority
	template <typename T>
	void GetHighestPriorityList(TArray<const T*>& OutList) const
	{
		if (IsEmpty())
			return;

		for (const FSecInstigatorObjectList& InstigatorList : PriorityList.Last().InstigatorList)
		{
			OutList.Add(CastChecked<T>(InstigatorList.ObjectType));
		}
	}

	// Get all objects in one list sorted by priority
	template <typename T>
	void GetSortedList(TArray<const T*>& OutList) const
	{
		if (IsEmpty())
			return;

		for (const FSecPriorityListContainer& ListContainer : PriorityList)
		{
			for (const FSecInstigatorObjectList& InstigatorList : ListContainer.InstigatorList)
			{
				OutList.Add(CastChecked<T>(InstigatorList.ObjectType));
			}
		}
	}

	template <typename T>
	T* GetHighestFromPriority(ESecScriptPriority InPriority) const
	{
		return GetHighestFromPriority<T>((int32)InPriority);
	}

	void Add(UObject* InObject, UObject* InInstigator, int32 InPriority);

	void Add(UObject* InObject, UObject* InInstigator, ESecScriptPriority Priority)
	{
		Add(InObject, InInstigator, (int32)Priority);
	}

	void RemoveByInstigator(UObject* InInstigator);
	void RemoveByObject(UObject* InObject);

private:
	void Internal_Add(UObject* InObject, FName InInstigatorName, int32 InPriority);

	UPROPERTY(Transient)
	TArray<FSecPriorityListContainer> PriorityList;
};
