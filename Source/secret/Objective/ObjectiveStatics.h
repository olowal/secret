#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "ObjectiveStatics.generated.h"

class AObjectiveSystemActor;
class UObjectiveSettings;

UENUM(BlueprintType)
enum class EObjectiveCallbackType : uint8
{
	EOCT_OnStart UMETA(DisplayName = "On Start"),
	EOCT_OnComplete UMETA(DisplayName = "On Complete"),
	EOCT_OnFail UMETA(DisplayName = "On Fail"),
};

USTRUCT(BlueprintType)
struct FObjectiveDelegateInfo
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, Category = Objective)
	UObjectiveSettings* Objective;
};

DECLARE_DYNAMIC_DELEGATE_OneParam(FObjectiveCallbackSignature, const FObjectiveDelegateInfo&, Objective);

USTRUCT(BlueprintType)
struct FObjectiveProgressInfo
{
	GENERATED_BODY()
public:
	FObjectiveProgressInfo(){}

	UPROPERTY(BlueprintReadOnly, Category = Objective)
	int32 Current;
	UPROPERTY(BlueprintReadOnly, Category = Objective)
	FName CounterName;
};

UCLASS()
class SECRET_API UObjectiveStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	static AObjectiveSystemActor* GetObjectiveSystemActor(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Objective, meta = (WorldContext = "WorldContextObject"))
	static void StartObjective(const UObject* WorldContextObject, UObjectiveSettings* Objective);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Objective, meta = (WorldContext = "WorldContextObject"))
	static void CompleteObjective(const UObject* WorldContextObject, UObjectiveSettings* Objective);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Objective, meta = (WorldContext = "WorldContextObject"))
	static void FailObjective(const UObject* WorldContextObject, UObjectiveSettings* Objective);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Objective, meta = (WorldContext = "WorldContextObject"))
	static void ProgressObjective(const UObject* WorldContextObject, UObjectiveSettings* Objective, int32 Value);

	UFUNCTION(BlueprintCallable, Category = Objective, meta = (WorldContext = "WorldContextObject"))
	static void SetObjectiveUIHidden(const UObject* WorldContextObject, bool bHidden);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Objective, meta = (WorldContext = "WorldContextObject"))
	static void CreateObjectiveDelegate(const UObject* WorldContextObject, UObjectiveSettings* Objective, EObjectiveCallbackType CallbackType, const FObjectiveCallbackSignature& Delegate);
};
