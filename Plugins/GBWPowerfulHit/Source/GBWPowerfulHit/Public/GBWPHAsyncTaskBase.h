// Copyright GanBowen 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/NoExportTypes.h"
#include "GBWPowerfulHitNetWorkObject.h"
#include "TimerManager.h"
#include "Tickable.h"
#include "GBWPHAsyncTaskBase.generated.h"

class UGBWPHAsyncActionObject;

DECLARE_DELEGATE_OneParam(FAsyncActionDelegate, UGBWPHAsyncActionObject*);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAsyncTaskDelegate, UGBWPHAsyncActionObject*, ActionObj);

UCLASS(BlueprintType, Blueprintable)
class UGBWPHAsyncActionObject : public UGBWPowerfulHitNetWorkObject
{
	GENERATED_BODY()
public:
	UGBWPHAsyncActionObject(const FObjectInitializer& objectInitializer);
	FAsyncActionDelegate OnStart;
	FAsyncActionDelegate OnAbort;
	FAsyncActionDelegate OnUpdate;
	FAsyncActionDelegate OnFinishd;

public:
	UFUNCTION(BlueprintCallable, Category = "Action")
		virtual bool OnActionStart(FString& ResultStr);
	UFUNCTION(BlueprintCallable, Category = "Action")
		virtual void OnActionAbort();
	UFUNCTION(BlueprintCallable, Category = "Action")
		virtual void OnActionUpdate();
	UFUNCTION(BlueprintCallable, Category = "Action")
		virtual void OnActionFinishd();

public:
	// Action status (BP Executable function)
	UFUNCTION(BlueprintCallable, Category = "Action")
		virtual bool Action_IsRunning()const;
	UFUNCTION(BlueprintCallable, Category = "Action")
		virtual bool Action_ExecutableStart()const;

protected:
	// End Action
	virtual void EndAction();
	virtual void UnBindAll();

	virtual void InitDelegateList();
	virtual bool Action_IsStarted()const;
	virtual bool Action_EventIsBinded()const;

protected:
	bool mActionStarted = false;
	TArray<FAsyncActionDelegate*> DelegateList;
};

UCLASS(BlueprintType)
class GBWPOWERFULHIT_API UGBWPHAsyncTaskBase : public UBlueprintAsyncActionBase
{
	GENERATED_UCLASS_BODY()

public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
		static UGBWPHAsyncTaskBase* CreateGBWPHAsyncTask(UGBWPHAsyncActionObject* ActionObj);

public:

	UPROPERTY(BlueprintAssignable)
		FAsyncTaskDelegate OnStart;

	UPROPERTY(BlueprintAssignable)
		FAsyncTaskDelegate OnAbort;

	UPROPERTY(BlueprintAssignable)
		FAsyncTaskDelegate OnUpdate;

	UPROPERTY(BlueprintAssignable)
		FAsyncTaskDelegate OnFinishd;

protected:
	void StartTask(UGBWPHAsyncActionObject* ActionObj);
	virtual void OnActionStart(UGBWPHAsyncActionObject* ActionObj);
	virtual void OnActionAbort(UGBWPHAsyncActionObject* ActionObj);
	virtual void OnActionUpdate(UGBWPHAsyncActionObject* ActionObj);
	virtual void OnActionFinishd(UGBWPHAsyncActionObject* ActionObj);
};


UCLASS(meta = (HideThen = true))
class GBWPOWERFULHIT_API UGBWPHAsync_SetTimer : public UBlueprintAsyncActionBase, public FTickableGameObject
{
	//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTimerHandleDelegate, FTimerHandle, TimerHandle);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FTimerHandleDelegate,
		FTimerHandle, TimerHandle,
		UObject*, ParameterObject,
		float, Time);
	GENERATED_BODY()

public:

	UGBWPHAsync_SetTimer();

	~UGBWPHAsync_SetTimer();

	// FTickableGameObject Begin
	virtual void Tick( float DeltaTime ) override;
	virtual ETickableTickType GetTickableTickType() const override
	{
		return ETickableTickType::Always;
	}
	virtual TStatId GetStatId() const override
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT( FMyTickableThing, STATGROUP_Tickables );
	}
	virtual bool IsTickableWhenPaused() const
	{
		return true;
	}
	virtual bool IsTickableInEditor() const
	{
		return false;
	}
	// FTickableGameObject End


public:

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true",
		WorldContext = "WorldContextObject",
		DisplayName = "SetTimerPro",
		ScriptName = "SetTimerPro",
		AdvancedDisplay = "InitialStartDelay, InitialStartDelayVariance"),
		Category = "GBW|Async")
		static UGBWPHAsync_SetTimer* SetTimer(const UObject* WorldContextObject,
			float Time,
			bool bLooping,
			float InitialStartDelay = 0.f,
			float InitialStartDelayVariance = 0.f,
			UObject* ParameterObject=nullptr);

public: 
	
	UPROPERTY(BlueprintAssignable)
		FTimerHandleDelegate Then;

	UPROPERTY(BlueprintAssignable)
	FTimerHandleDelegate Update;
	
	UPROPERTY(BlueprintAssignable)
		FTimerHandleDelegate Completed;

private:

	virtual void Activate() override;
	
	UFUNCTION()
		void CompletedEvent();
	
	FTimerHandle SetTimerDelegate(FTimerDynamicDelegate Delegate,
		float Time,
		bool bLooping,
		float InitialStartDelay = 0.f,
		float InitialStartDelayVariance = 0.f);
	
	void PreGarbageCollect();

	// The last frame number we were ticked.
	// We don't want to tick multiple times per frame 
	uint32 LastFrameNumberWeTicked = INDEX_NONE;

private:
	const UObject* WorldContextObject;
	FTimerHandle TimerHandle;
	UObject* ParameterObjectCache;
	float Timer = 0.0f;
	bool bIsCompleted = false;
	
	UWorld*  World;
};

