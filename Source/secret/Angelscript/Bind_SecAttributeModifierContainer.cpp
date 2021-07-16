#include "AngelscriptBinds.h"
#include "Attribute/SecAttributeModifier.h"

FAngelscriptBinds::FBind Bind_SecAttributeModifierContainer(FAngelscriptBinds::EOrder::Late, []
{
		auto FSecAttributeModifierContainer_ = FAngelscriptBinds::ExistingClass("FSecAttributeModifierContainer");
		FSecAttributeModifierContainer_.Constructor("void f(float InModifier, TSubclassOf<USecAttribute> InAttribute, ESecAttributeModifierType InModifierType, TSubclassOf<UAttributeModifier> InCustomModifierClass)", [](
			FSecAttributeModifierContainer* Address, 
			float InModifier, 
			TSubclassOf<USecAttribute> InAttribute,
			ESecAttributeModifierType InModifierType,
			TSubclassOf<UAttributeModifier> InCustomModifierClass
			)
		{
				new(Address) FSecAttributeModifierContainer(InModifier, InAttribute, InModifierType, InCustomModifierClass);
		});

		FSecAttributeModifierContainer_.Constructor("void f(float InModifier, TSubclassOf<USecAttribute> InAttribute, ESecAttributeModifierType InModifierType)", [](
			FSecAttributeModifierContainer* Address,
			float InModifier,
			TSubclassOf<USecAttribute> InAttribute,
			ESecAttributeModifierType InModifierType
			)
			{
				new(Address) FSecAttributeModifierContainer(InModifier, InAttribute, InModifierType);
			});
	});