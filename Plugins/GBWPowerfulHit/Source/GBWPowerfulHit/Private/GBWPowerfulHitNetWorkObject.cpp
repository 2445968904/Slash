// Copyright GanBowen 2022. All Rights Reserved.


#include "GBWPowerfulHitNetWorkObject.h"

void UGBWPowerfulHitNetWorkObject::SetWorldContext(UObject* NewWorldContext)
{
	WorldContext = new FWorldContext();
	WorldContext->SetCurrentWorld(NewWorldContext->GetWorld());
}

UObject* UGBWPowerfulHitNetWorkObject::GetWorldContextObject()
{
	return WorldContext->World();
}

class UWorld* UGBWPowerfulHitNetWorkObject::GetWorld() const
{
	return WorldContext ? WorldContext->World() : NULL;
}