#pragma once

#include "Components/ActorComponent.h"
#include "SecAttributeStatics.h"
#include "SecAttributeModifierSheet.h"
#include "Utils/SecPriorityList.h"
#include "SecActionState.h"
#include "SecAttributeComponent.generated.h"

class USecAttributeSheet;
class USecAttributeModifierSheet;
class USecAttribute;

UENUM(BlueprintType)
enum class EAttributeSetType : uint8
{
	Override,
	Add,
	Sub,
	Mul,
	Div
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSecAttributeInstanceDelegate, USecAttribute*, Attribute, USecAttributeComponent*, AttributeComponent);

UCLASS()
class SECRET_API USecAttributeComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	USecAttributeComponent(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = Attribute)
	void SetVectorAttribute(FName AttributeName, const FVector& Attribute);
	UFUNCTION(BlueprintCallable, Category = Attribute)
	void SetNumberAttribute(FName AttributeName, float Attribute);
	UFUNCTION(BlueprintCallable, Category = Attribute)
	void SetValueAttribute(FName AttributeName, int32 Attribute);
	UFUNCTION(BlueprintCallable, Category = Attribute)
	void SetActionAttribute(FName AttributeName, ESecActionState ActionState);
	UFUNCTION(BlueprintCallable, Category = Attribute)
	void SetObjectAttribute(FName AttributeName, UObject* ObjectAttribute);

	UFUNCTION(BlueprintCallable, Category = Attribute)
	void ApplyAttributeSheet(USecAttributeSheet* AttributeSheet, UObject* Instigator);
	UFUNCTION(BlueprintCallable, Category = Attribute)
	void RemoveAttributeSheetByInstigator(UObject* Instigator);
	UFUNCTION(BlueprintCallable, Category = Attribute)
	void RemoveAttributeSheetByAsset(USecAttributeSheet* AttributeSheet);

	template <class AttributeClass>
	AttributeClass* AddAttribute()
	{
		return CastChecked<AttributeClass>(AddAttribute(AttributeClass::StaticClass()));
	}

	template <class AttributeClass>
	AttributeClass* FindAttribute()
	{
		return GetAttributeByClass(AttributeClass::StaticClass());
	}

	// Adding from a sheet must call Setup manually later because attributes may require other attributes to exist.
	USecAttribute* AddAttributeFromSheet(TSubclassOf<USecAttribute> InAttributeClass);
	UFUNCTION(BlueprintPure, Category = Attribute)
	USecAttribute* GetAttributeByClass(TSubclassOf<USecAttribute> InAttributeClass) const;
	UFUNCTION(BlueprintCallable, Category = Attribute)
	USecAttribute* GetOrCreate(TSubclassOf<USecAttribute> InAttributeClass);

	bool DoesAttributeExist(TSubclassOf<USecAttribute> InAttributeClass) const;

	UFUNCTION(BlueprintCallable, Category = Attribute)
	void ApplyModifierSheet(USecAttributeModifierSheet* AttributeModifierSheet, UObject* Instigator, ESecScriptPriority Priority);
	UFUNCTION(BlueprintCallable, Category = Attribute)
	void RemoveModifierSheetByInstigator(UObject* Instigator);
	UFUNCTION(BlueprintCallable, Category = Attribute)
	void RemoveModifierSheetByAsset(USecAttributeModifierSheet* AttributeSheet);
	
	UFUNCTION(BlueprintCallable, Category = Attribute)
	void ApplyModifier(const FSecAttributeModifierContainer& AttributeModifier, UObject* Instigator, ESecScriptPriority Priority);
	// Applies a modifier to the base attribute value making it permanent
	UFUNCTION(BlueprintCallable, Category = Attribute)
	void ApplyModifierAndForget(const FSecAttributeModifierContainer& AttributeModifier);
	UFUNCTION(BlueprintCallable, Category = Attribute)
	void RemoveModifierByInstigator(UObject* Instigator);

	UFUNCTION(BlueprintPure, Category = Attribute)
	bool GetActionAttribute(FName AttributeName) const;
	UFUNCTION(BlueprintPure, Category = Attribute)
	FVector GetVectorAttribute(FName AttributeName) const;
	UFUNCTION(BlueprintPure, Category = Attribute)
	float GetNumberAttribute(FName AttributeName) const;
	UFUNCTION(BlueprintPure, Category = Attribute)
	int32 GetValueAttribute(FName AttributeName) const;
	UFUNCTION(BlueprintPure, Category = Attribute)
	UObject* GetObjectAttribute(FName AttributeName) const;

	UFUNCTION(BlueprintPure, Category = Attribute)
	bool DoesVectorAttributeExist(FName AttributeName) const;
	UFUNCTION(BlueprintPure, Category = Attribute)
	bool DoesNumberAttributeExist(FName AttributeName) const;
	UFUNCTION(BlueprintPure, Category = Attribute)
	bool DoesValueAttributeExist(FName AttributeName) const;
	UFUNCTION(BlueprintPure, Category = Attribute)
	bool DoesObjectAttributeExist(FName AttributeName) const;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(BlueprintAssignable)
	FSecAttributeInstanceDelegate OnAttributeInstanced;

private:
	USecAttribute* AddAttribute(TSubclassOf<USecAttribute> InAttributeClass);

	bool Internal_GetActionAttribute(const TMap<FName, ESecActionState>& InActionAttributes, FName AttributeName) const;

	float ApplyNumberAttributeModifiers(FName AttributeName, float Attribute) const;

	TMap<FName, FVector> VectorAttributes;

	UPROPERTY(Transient)
	TMap<FName, UObject*> ObjectAttributes;

	TMap<FName, float> NumberAttributes;

	TMap<FName, int32> ValueAttributes;
	
	TMap<FName, ESecActionState> ActionAttributes;

	TMap<FName, ESecActionState> LastActionAttributes;

	UPROPERTY(Transient)
	TArray<USecAttribute*> ListOfAttributes;

	UPROPERTY(Transient)
	FSecPriorityList ListOfModifiers;
};