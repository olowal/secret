#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ObjectiveUIStatics.generated.h"

class UObjectiveSettings;

USTRUCT(BlueprintType)
struct FObjectiveUICounterInfo
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, Category = Objective)
	FText CounterDescription;
	UPROPERTY(BlueprintReadOnly, Category = Objective)
	int32 Current = 0;
	UPROPERTY(BlueprintReadOnly, Category = Objective)
	int32 Target = 0;
	UPROPERTY(BlueprintReadOnly, Category = Objective)
	FName CounterId = NAME_None;
};

USTRUCT(BlueprintType)
struct FObjectiveUITimerInfo
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, Category = Objective)
	FText TimerDescription;
	UPROPERTY(BlueprintReadOnly, Category = Objective)
	float Current = 0.0f;
	UPROPERTY(BlueprintReadOnly, Category = Objective)
	float Target = 0.0f;
	UPROPERTY(BlueprintReadOnly, Category = Objective)
	FName TimerId = NAME_None;
};

USTRUCT(BlueprintType)
struct FObjectiveCounterUIInfo
{
	GENERATED_BODY()
public:
	FObjectiveCounterUIInfo(){}
	FObjectiveCounterUIInfo(int32 InStart, int32 InTarget, int32 InCurrent) :
		Start(InStart), Target(InTarget), Current(InCurrent) {}

	UPROPERTY(BlueprintReadOnly, Category = Objective)
	int32 Start = 0;

	UPROPERTY(BlueprintReadOnly, Category = Objective)
	int32 Target = 0;

	UPROPERTY(BlueprintReadOnly, Category = Objective)
	int32 Current = 0;
};

USTRUCT(BlueprintType)
struct FObjectiveTimerUIInfo
{
	GENERATED_BODY()
public:
	FObjectiveTimerUIInfo(){}
	FObjectiveTimerUIInfo(float InStart, float InTarget, float InCurrent) :
		Start(InStart), Target(InTarget), Current(InCurrent) {}

	UPROPERTY(BlueprintReadOnly, Category = Objective)
	float Start = 0;

	UPROPERTY(BlueprintReadOnly, Category = Objective)
	float Target = 0;

	UPROPERTY(BlueprintReadOnly, Category = Objective)
	float Current = 0;
};

USTRUCT(BlueprintType)
struct FObjectiveUIInfo
{
	GENERATED_BODY()
public:
	// Original settings for comparison purpose.
	UPROPERTY(BlueprintReadOnly, Category = Objective)
	UObjectiveSettings* Settings;

	UPROPERTY(BlueprintReadOnly, Category = Objective)
	FText Description;

	UPROPERTY(BlueprintReadOnly, Category = Objective)
	bool bUseCounter = false;

	UPROPERTY(BlueprintReadOnly, Category = Objective)
	FObjectiveCounterUIInfo Counter;

	UPROPERTY(BlueprintReadOnly, Category = Objective)
	bool bUseTimer = false;

	UPROPERTY(BlueprintReadOnly, Category = Objective)
	FObjectiveTimerUIInfo Timer;
};
