#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "SecUtilityStatics.generated.h"

UCLASS()
class SECRET_API USecUtilityStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	// Returns the enum as a string, excluding the name of the enum.
	template <typename EnumType>
	static FString EnumToString(EnumType Enum)
	{
		FString String = UEnum::GetValueAsString(Enum);

		int32 Index = -1;
		if (String.FindChar(':', Index) && String.Len() > (Index + 1))
		{
			String.RemoveAt(0, Index + 2);
		}

		return String;
	}
};
