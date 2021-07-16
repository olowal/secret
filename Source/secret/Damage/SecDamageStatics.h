#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "SecDamageStatics.generated.h"

UCLASS(BlueprintType, Blueprintable)
class SECRET_API USecDamageType : public UObject
{
	GENERATED_BODY()
public:
};

UENUM(BlueprintType)
enum class ESecDamageType : uint8
{
	Point,
	Radial,
	Cone
};

USTRUCT(BlueprintType)
struct FSecDamagePrediction
{
	GENERATED_BODY()
public:

};

USTRUCT(BlueprintType)
struct FSecDamageOutput
{
	GENERATED_BODY()
public:
	UPROPERTY(Transient, BlueprintReadWrite, Category = Damage)
	USecDamageType* DamageType = nullptr;
	UPROPERTY(Transient, BlueprintReadWrite, Category = Damage)
	float DamageDealt = 0.0f;
	UPROPERTY()
	int32 SyncTag = 0;
};

USTRUCT(BlueprintType)
struct FSecDamageInput
{
	GENERATED_BODY()
public:
	FSecDamageInput() {}
	FSecDamageInput(const FSecDamageOutput& Output, const FSecDamageInput& Input)
	{
		DamageOrigin = Input.DamageOrigin;
		DamageDealer = Input.DamageDealer;
		DamageType = Output.DamageType;
		Damage = Output.DamageDealt;
	}
	FSecDamageInput(float InDamage, const FVector& InDamageOrigin, AActor* InDamageDealer)
		:
		DamageOrigin(InDamageOrigin),
		DamageDealer(InDamageDealer),
		Damage(InDamage)
	{}
	FSecDamageInput(float InDamage, const FVector& InDamageOrigin, AActor* InDamageDealer, USecDamageType* InDamageType)
		:
		DamageOrigin(InDamageOrigin),
		DamageDealer(InDamageDealer),
		DamageType(InDamageType),
		Damage(InDamage)
	{}
	
	~FSecDamageInput() {}


	UPROPERTY(Transient, BlueprintReadOnly)
	FVector DamageOrigin = FVector::ZeroVector;
	// The actor that dealt the damage
	UPROPERTY(Transient, BlueprintReadOnly)
	AActor* DamageDealer = nullptr;
	UPROPERTY(Transient, BlueprintReadOnly)
	USecDamageType* DamageType = nullptr;
	UPROPERTY(Transient, BlueprintReadOnly)
	float Damage = 0.0f;

	int32 SyncTag = 0;
};

USTRUCT(BlueprintType)
struct FSecPointDamage
{
	GENERATED_BODY()
public:
	UPROPERTY()
	FSecDamageInput DamageInput;
	UPROPERTY()
	FVector DamageLocation;
};

USTRUCT(BlueprintType)
struct FSecRadialDamage
{
	GENERATED_BODY()
public:
	UPROPERTY()
	FSecDamageInput DamageInput;
	UPROPERTY()
	FVector Origin;
	UPROPERTY()
	float Radius;
};

USTRUCT(BlueprintType)
struct FSecConeDamage
{
	GENERATED_BODY()
public:
	UPROPERTY()
	FSecDamageInput DamageInput;
	UPROPERTY()
	FVector Origin;
	UPROPERTY()
	FVector Direction;
	UPROPERTY()
	float Angle;
};

UCLASS(BlueprintType, Blueprintable, abstract)
class USecDamageModifier : public UObject
{
	GENERATED_BODY()
public:

	void Setup();
	// Runs the first time this Modifier is applied.
	UFUNCTION(BlueprintImplementableEvent, Category = Damage, meta = (DisplayName = "Setup"))
	void BP_Setup();

	void OnApply();
	// Runs whenever the Damage Modifier is applied.
	UFUNCTION(BlueprintImplementableEvent, Category = Damage, meta = (DisplayName = "On Apply"))
	void BP_OnApply();

	void OnRemove();
	// Runs when the Damage Modifier is removed, after first being applied.
	UFUNCTION(BlueprintImplementableEvent, Category = Damage, meta = (DisplayName = "On Remove"))
	void BP_OnRemove();

	void ProcessDamageOutput(const FSecDamageInput& Input, FSecDamageOutput& Output) const;

	UFUNCTION(BlueprintImplementableEvent, Category = Damage)
	void BP_ProcessDamageOutput(const FSecDamageInput& Input, FSecDamageOutput& Output) const;

	UFUNCTION(BlueprintPure, Category = Damage)
	bool IsApplied() const;

	UPROPERTY(Transient, BlueprintReadOnly)
	AActor* Owner = nullptr;

private:
	bool bIsApplied = false;
};

class USecTakeDamageComponent;
class USecDealDamageComponent;

UCLASS()
class SECRET_API USecDamageStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	static USecTakeDamageComponent* GetOrCreateTakeDamageComponent(AActor* TargetActor);
	static USecDealDamageComponent* GetOrCreateDealDamageComponent(AActor* TargetActor);
};

UCLASS(meta = (ScriptMixin = "FSecLineDamage"))
class SECRET_API USecLineDamageStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure, Category = "Damage")
	static void DealDamageTo(const FSecPointDamage& DamageType_Line, AActor* TargetActor);
};
