#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "SecMath.generated.h"

UCLASS(BlueprintType)
class SECRET_API USecMathStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// Increment an index from, for example, a TArray and return 0 if it passes max.
	UFUNCTION(BlueprintPure, Category = Math)
	static int32 IncrementIndex(int32 IndexCurrent, int32 IndexMax);

	template <typename T>
	static void Swap(T& A, T& B)
	{
		T Temp = A;
		A = B;
		B = Temp;
	}

	template <typename T>
	static void BubbleSort(TArray<T>& List)
	{
		const int32 Num = List.Num();
		if (Num <= 1)
			return;

		if (Num == 2)
		{
			if (List[0] > List[1])
				Swap<T>(List[0], List[1]);
			return;
		}

		bool bDone = false;

		while (!bDone)
		{
			bDone = true;
			for (int32 Index = 0; Index < (Num - 1); ++Index)
			{
				if (List[Index] > List[Index + 1])
				{
					bDone = false;
					Swap<T>(List[Index], List[Index + 1]);
				}
			}
		}
	}

	// Smallest first, largest last
	template <typename T>
	static void BubbleSortReversed(TArray<T>& List)
	{
		const int32 Num = List.Num();
		if (Num <= 1)
			return;

		if (Num == 2)
		{
			if (List[0] < List[1])
				Swap<T>(List[0], List[1]);
			return;
		}

		bool bDone = false;

		while (!bDone)
		{
			bDone = true;
			for (int32 Index = 0; Index < (Num - 1); ++Index)
			{
				if (List[Index] < List[Index + 1])
				{
					bDone = false;
					Swap<T>(List[Index], List[Index + 1]);
				}
			}
		}
	}
};
