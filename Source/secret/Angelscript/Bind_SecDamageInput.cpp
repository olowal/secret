#include "AngelscriptBinds.h"
#include "Damage/SecDamageStatics.h"

FAngelscriptBinds::FBind Bind_SecDamageInput(FAngelscriptBinds::EOrder::Late, []
	{
		auto FSecDamageInput_ = FAngelscriptBinds::ExistingClass("FSecDamageInput");
		FSecDamageInput_.Constructor("void f(float Damage, FVector DamageOrigin, AActor DamageDealer)", [](
			FSecDamageInput* Address,
			float Damage,
			const FVector& DamageOrigin,
			AActor* DamageDealer
			)
			{
				new(Address) FSecDamageInput(Damage, DamageOrigin, DamageDealer);
			});

		FSecDamageInput_.Constructor("void f(float Damage, FVector DamageOrigin, AActor DamageDealer, USecDamageType DamageType)", [](
			FSecDamageInput* Address,
			float Damage,
			const FVector& DamageOrigin,
			AActor* DamageDealer,
			USecDamageType* DamageType
			)
			{
				new(Address) FSecDamageInput(Damage, DamageOrigin, DamageDealer, DamageType);
			});
	});