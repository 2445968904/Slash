// Copyright GanBowen 2022. All Rights Reserved.


#include "GBWPowerfulHitFuncLib.h"

TArray<FName> UGBWPowerfulHitFuncLib::GBWGetSectionNameFromMontage(UAnimMontage* Montage)
{
	TArray<FName> result;
	int maxIndex = Montage->CompositeSections.Num() - 1;
	for (int i = 0; i <= maxIndex; i++)
	{
		result.Add(Montage->CompositeSections[i].SectionName);
	}

	return result;
}

void UGBWPowerfulHitFuncLib::GBWCloneObject(UObject* ObjectIn, UObject*& Object)
{
	Object = DuplicateObject<UObject>(ObjectIn, Object);
}
