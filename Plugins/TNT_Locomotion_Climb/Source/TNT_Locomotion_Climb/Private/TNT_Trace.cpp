// Copyright TNT,2022. All Rights Reserved.


#include "TNT_Trace.h"
#include "Engine.h"
#include "PhysicsEngine/PhysicsSettings.h"
#include "DrawDebugHelpers.h"

bool UTNT_Trace::TNTCustomCapsuleTraceForObjectRotation(UObject* WorldContext, const FVector Start, const FVector End, float Radius,
	float HalfHeight, const FRotator Rotation, const TArray<TEnumAsByte<EObjectTypeQuery>>& ObjectTypes,
	bool Complex, const TArray<AActor*>& Ignore, EDrawDebugTrace::Type DrawDebugType, FHitResult& OutHit, bool IgnoreSelf,
	FLinearColor TraceColor, FLinearColor TraceHitColor, float DrawTime)
{
	FCollisionQueryParams Params = TNTCustomConfigurenParams(TEXT("CapsuleTraceSingleForObject"), Complex, Ignore, IgnoreSelf, WorldContext);
	FCollisionObjectQueryParams ObjectParams = TNTCustomConfigureObjectParams(ObjectTypes);
	if (ObjectParams.IsValid() == false) return false;

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull);
	bool const bHit = World ? World->SweepSingleByObjectType(OutHit, Start, End, Rotation.Quaternion(), ObjectParams, FCollisionShape::MakeCapsule(Radius, HalfHeight), Params) : false;

#if ENABLE_DRAW_DEBUG
	if (DrawDebugType != EDrawDebugTrace::None)
	{
		bool bPersistent = DrawDebugType == EDrawDebugTrace::Persistent;
		float LifeTime = (DrawDebugType == EDrawDebugTrace::ForDuration) ? DrawTime : 0.f;

		if (bHit && OutHit.bBlockingHit)
		{
			::DrawDebugCapsule(World, End, HalfHeight, Radius, Rotation.Quaternion(), TraceColor.ToFColor(true), bPersistent, LifeTime);
			::DrawDebugCapsule(World, OutHit.Location, HalfHeight, Radius, Rotation.Quaternion(), TraceColor.ToFColor(true), bPersistent, LifeTime);
			::DrawDebugLine(World, End, OutHit.Location, TraceColor.ToFColor(true), bPersistent, LifeTime);
			::DrawDebugPoint(World, OutHit.ImpactPoint, 16.f, TraceColor.ToFColor(true), bPersistent, LifeTime);

			::DrawDebugCapsule(World, End, HalfHeight, Radius, Rotation.Quaternion(), TraceHitColor.ToFColor(true), bPersistent, LifeTime);
			::DrawDebugLine(World, OutHit.Location, End, TraceHitColor.ToFColor(true), bPersistent, LifeTime);
		}
		else
		{
			::DrawDebugCapsule(World, End, HalfHeight, Radius, Rotation.Quaternion(), TraceColor.ToFColor(true), bPersistent, LifeTime);
			::DrawDebugCapsule(World, End, HalfHeight, Radius, Rotation.Quaternion(), TraceColor.ToFColor(true), bPersistent, LifeTime);
			::DrawDebugLine(World, End, End, TraceColor.ToFColor(true), bPersistent, LifeTime);
		}
	}
#endif

	return bHit;
}

bool UTNT_Trace::TNTCustomCapsuleMultiTraceForObjectRotation(UObject* WorldContext, const FVector Start,
	const FVector End, float Radius, float HalfHeight, const FRotator Orientation,
	const TArray<TEnumAsByte<EObjectTypeQuery>>& ObjectTypes, bool bTraceComplex, const TArray<AActor*>& ActorsToIgnore,
	EDrawDebugTrace::Type DrawDebugType, TArray<FHitResult>& OutHits, bool bIgnoreSelf, FLinearColor TraceColor,
	FLinearColor TraceHitColor, float DrawTime)
{
	FCollisionQueryParams Params = TNTCustomConfigurenParams(TEXT("TNTCapsuleMultiTraceForObjectsWithRotation"), bTraceComplex, ActorsToIgnore, bIgnoreSelf, WorldContext);

	FCollisionObjectQueryParams ObjectParams = TNTCustomConfigureObjectParams(ObjectTypes);
	if (ObjectParams.IsValid() == false) return false;

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull);
	bool const bHit = World ? World->SweepMultiByObjectType(OutHits, Start, End, Orientation.Quaternion(), ObjectParams, FCollisionShape::MakeCapsule(Radius, HalfHeight), Params) : false;

	return bHit;
}

FCollisionQueryParams UTNT_Trace::TNTCustomConfigurenParams(FName Name, bool Complex, const TArray<AActor*>& Ignore,
                                                            bool IgnoreSelf, UObject* WorldContext)
{
	FCollisionQueryParams Params(Name, SCENE_QUERY_STAT_ONLY(KismetTraceUtils), Complex);
	Params.bReturnPhysicalMaterial = true;
	Params.bReturnFaceIndex = !UPhysicsSettings::Get()->bSuppressFaceRemapTable; 
	Params.AddIgnoredActors(Ignore);
	if (IgnoreSelf)
	{
		AActor* IgnoreActor = Cast<AActor>(WorldContext);
		if (IgnoreActor)
		{
			Params.AddIgnoredActor(IgnoreActor);
		}
		else
		{
			UObject* CurrentObject = WorldContext;
			while (CurrentObject)
			{
				CurrentObject = CurrentObject->GetOuter();
				IgnoreActor = Cast<AActor>(CurrentObject);
				if (IgnoreActor)
				{
					Params.AddIgnoredActor(IgnoreActor);
					break;
				}
			}
		}
	}

	return Params;
}

FCollisionObjectQueryParams UTNT_Trace::TNTCustomConfigureObjectParams(const TArray<TEnumAsByte<EObjectTypeQuery>>& Types)
{
	TArray<TEnumAsByte<ECollisionChannel>> CollisionObjectTraces;
	CollisionObjectTraces.AddUninitialized(Types.Num());

	for (auto Iter = Types.CreateConstIterator(); Iter; ++Iter)
	{
		CollisionObjectTraces[Iter.GetIndex()] = UEngineTypes::ConvertToCollisionChannel(*Iter);
	}

	FCollisionObjectQueryParams ObjectParams;
	for (auto Iter = CollisionObjectTraces.CreateConstIterator(); Iter; ++Iter)
	{
		const ECollisionChannel & Channel = (*Iter);
		if (FCollisionObjectQueryParams::IsValidObjectQuery(Channel))
		{
			ObjectParams.AddObjectTypesToQuery(Channel);
		}
	}

	return ObjectParams;
}
