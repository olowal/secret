#pragma once

#include "Kismet/BlueprintAsyncActionBase.h"
#include "ObjectiveStatics.h"
#include "ObjectiveCallbackProxy.generated.h"

class UObjectiveBase;
class UObjectiveSettings;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FObjectiveProxySignature);

UCLASS(BlueprintType, Blueprintable, meta = (ExposedAsyncProxy = Objective))
class SECRET_API UCreateObjectiveTask : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:

	template <class T>
	static T* CreateObjectiveProxy(const UObject* WorldContextObject, TSubclassOf<UObjectiveBase> ObjectiveClass)
	{
		if (!HasAuthority(WorldContextObject))
		{
			return nullptr;
		}

		T* ObjectiveProxy = NewObject<UCreateObjectiveTask>();
		return ObjectiveProxy;
	}

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, meta=(BlueprintInternalUseOnly = "true", WorldContext="WorldContextObject"), Category = Objective)
	static UCreateObjectiveTask* CreateObjective(const UObject* WorldContextObject, UObjectiveSettings* Objective);

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, meta = (WorldContext = "WorldContextObject"), Category = Objective)
    void Start(const UObject* WorldContextObject);
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, meta = (WorldContext = "WorldContextObject"), Category = Objective)
    void Complete(const UObject* WorldContextObject);
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, meta = (WorldContext = "WorldContextObject"), Category = Objective)
    void Fail(const UObject* WorldContextObject);
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, meta = (WorldContext = "WorldContextObject"), Category = Objective)
    void Progress(const UObject* WorldContextObject, int32 Value);

	UPROPERTY(BlueprintAssignable)
	FObjectiveProxySignature OnStart;
	
	UPROPERTY(BlueprintAssignable)
	FObjectiveProxySignature OnComplete;

	UPROPERTY(BlueprintAssignable)
	FObjectiveProxySignature OnFail;

	FObjectiveCallbackSignature Internal_OnStart;
	FObjectiveCallbackSignature Internal_OnComplete;
	FObjectiveCallbackSignature Internal_OnFail;

	UPROPERTY(Transient)
	UObjectiveBase* Objective;

	void RegisterDelegates();

	UFUNCTION()
	void HandleOnStart();
	UFUNCTION()
	void HandleOnComplete();
	UFUNCTION()
	void HandleOnFail();

	const static FName HandleOnStartName;
	const static FName HandleOnCompleteName;
	const static FName HandleOnFailName;

protected:
	static bool HasAuthority(const UObject* WorldContextObject);

};
