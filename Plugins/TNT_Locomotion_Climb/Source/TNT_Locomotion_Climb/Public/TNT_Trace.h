// Copyright TNT,2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/KismetSystemLibrary.h"
#include "CollisionQueryParams.h"
#include "TNT_Trace.generated.h"

/**
 * 
 */
UCLASS()
class TNT_LOCOMOTION_CLIMB_API UTNT_Trace : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable,meta = (bIgnoreSelf = "true", WorldContext = "WorldContext", AutoCreateRefTerm = "Ignore"), Category = "TNT")
	static bool TNTCustomCapsuleTraceForObjectRotation(UObject* WorldContext, const FVector Start, const FVector End, float Radius, float HalfHeight, const FRotator Rotation, const TArray<TEnumAsByte<EObjectTypeQuery> > & ObjectTypes, bool Complex, const TArray<AActor*>& Ignore, EDrawDebugTrace::Type DrawDebugType, FHitResult& OutHit, bool IgnoreSelf, FLinearColor TraceColor = FLinearColor::Red, FLinearColor TraceHitColor = FLinearColor::Green, float DrawTime = 5.0f);

	UFUNCTION(BlueprintCallable,meta = (bIgnoreSelf = "true", WorldContext = "WorldContext", AutoCreateRefTerm = "Ignore"), Category = "TNT")
	static bool TNTCustomCapsuleMultiTraceForObjectRotation(UObject* WorldContext, const FVector Start, const FVector End, float Radius, float HalfHeight, const FRotator Rotation, const TArray<TEnumAsByte<EObjectTypeQuery> > & ObjectTypes, bool Complex, const TArray<AActor*>& Ignore, EDrawDebugTrace::Type DrawDebugType, TArray<FHitResult>& OutHits, bool bIgnoreSelf, FLinearColor TraceColor = FLinearColor::Red, FLinearColor TraceHitColor = FLinearColor::Green, float DrawTime = 5.0f);
private:
	static inline FCollisionQueryParams TNTCustomConfigurenParams(FName Name, bool Complex, const TArray<AActor*>& Ignore, bool IgnoreSelf, UObject* WorldContext);
	static inline FCollisionObjectQueryParams TNTCustomConfigureObjectParams(const TArray<TEnumAsByte<EObjectTypeQuery> > & Types);

};
