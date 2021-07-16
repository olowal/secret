#include "Engine/Engine.h"

#include "Animation/AnimMontage.h"

#include "AngelscriptCodeModule.h"

#include "UObject/UObjectIterator.h"

#include "AngelscriptManager.h"
#include "AngelscriptType.h"
#include "AngelscriptBinds.h"

#include "StartAngelscriptHeaders.h"
#include "as_objecttype.h"
#include "EndAngelscriptHeaders.h"

/**
 * Binds default methods from AnimMontage
 */
FAngelscriptBinds::FBind Bind_UAnimMontage_Base((int32)FAngelscriptBinds::EOrder::Late - 1, []
	{
		auto UAnimMontage_ = FAngelscriptBinds::ExistingClass("UAnimMontage");
		
		UAnimMontage_.Method("int GetSectionIndex(FName InSectionName) const",
			[](UAnimMontage* AnimMontage, FName InSectionIndex)
			{
				return AnimMontage->GetSectionIndex(InSectionIndex);
			});
		
		UAnimMontage_.Method("float GetSectionLength(int32 InSectionIndex) const",
			[](UAnimMontage* AnimMontage, int32 InSectionIndex)
			{
				return AnimMontage->GetSectionLength(InSectionIndex);
			});

		UAnimMontage_.Method("float GetSectionLengthByName(FName InSectionName) const",
			[](UAnimMontage* AnimMontage, FName InSectionName)
			{
				const int32 SectionIndex = AnimMontage->GetSectionIndex(InSectionName);
				return AnimMontage->GetSectionLength(SectionIndex);
			});
			
	});
