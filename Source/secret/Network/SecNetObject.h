#pragma once

#include "UObject/Object.h"
#include "SecNetObject.generated.h"

UCLASS(abstract, BlueprintType, Blueprintable)
class SECRET_API USecNetObject : public UObject
{
	GENERATED_BODY()
public:

	virtual int32 GetFunctionCallspace(UFunction* Function, FFrame* Stack) override;
	virtual bool CallRemoteFunction(UFunction* Function, void* Parameters, FOutParmRec* OutParms, FFrame* Stack) override;
	virtual bool IsSupportedForNetworking() const override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
};
