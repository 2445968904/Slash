// Copyright GanBowen 2022. All Rights Reserved.


#include "GBWPHAsyncTaskBase.h"
#include "Engine/Engine.h"

UGBWPHAsyncActionObject::UGBWPHAsyncActionObject(const FObjectInitializer& objectInitializer)
{
	InitDelegateList();
}

bool UGBWPHAsyncActionObject::OnActionStart(FString& ResultStr)
{
	ResultStr.Reset();
	bool local_bResault = false;

	if (Action_ExecutableStart())
	{

		mActionStarted = true;
		OnStart.ExecuteIfBound(this);

		local_bResault = true;
	}
	else {
		local_bResault = false;
		if (Action_IsStarted())
		{
			ResultStr.Append(FString(TEXT("Action is Started.\n")));
		}
		if (!Action_EventIsBinded())
		{
			ResultStr.Append(FString(TEXT("Action is not bind to anything Task")));
		}
	}
	return local_bResault;
}

void UGBWPHAsyncActionObject::OnActionAbort()
{
	if (Action_IsRunning())
	{
		OnAbort.ExecuteIfBound(this);
		EndAction();
	}
}

void UGBWPHAsyncActionObject::OnActionUpdate()
{
	if (Action_IsRunning())
	{
		OnUpdate.ExecuteIfBound(this);
	}
}
void UGBWPHAsyncActionObject::OnActionFinishd()
{
	if (Action_IsRunning())
	{
		OnFinishd.ExecuteIfBound(this);
		EndAction();
	}
}

void UGBWPHAsyncActionObject::EndAction()
{
	if (Action_IsRunning())
	{
		UnBindAll();
		mActionStarted = false;
	}
}

void UGBWPHAsyncActionObject::UnBindAll()
{
	for (auto& DeleIndex : DelegateList)
	{
		if (DeleIndex->IsBound())
		{
			DeleIndex->Unbind();
		}
	}
}

void UGBWPHAsyncActionObject::InitDelegateList()
{
	if (!Action_IsStarted())
	{
		DelegateList.AddUnique(&OnStart);
		DelegateList.AddUnique(&OnAbort);
		DelegateList.AddUnique(&OnUpdate);
		DelegateList.AddUnique(&OnFinishd);
	}
}

bool UGBWPHAsyncActionObject::Action_EventIsBinded()const
{
	bool EventIsBinded = true;
	for (auto& DeleIndex : DelegateList)
	{
		if (!DeleIndex->IsBound())
			EventIsBinded = false;
	}
	return EventIsBinded;
}

bool UGBWPHAsyncActionObject::Action_IsStarted()const
{
	return mActionStarted;
}

bool UGBWPHAsyncActionObject::Action_IsRunning()const
{
	return Action_IsStarted() && Action_EventIsBinded();
}

bool UGBWPHAsyncActionObject::Action_ExecutableStart()const
{
	return !Action_IsStarted() && Action_EventIsBinded();
}

UGBWPHAsyncTaskBase::UGBWPHAsyncTaskBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	if (HasAnyFlags(RF_ClassDefaultObject) == false)
	{
		AddToRoot();
	}
}

UGBWPHAsyncTaskBase* UGBWPHAsyncTaskBase::CreateGBWPHAsyncTask(UGBWPHAsyncActionObject* ActionObj)
{
	UGBWPHAsyncTaskBase* AsyncActionTask = NewObject<UGBWPHAsyncTaskBase>();
	AsyncActionTask->StartTask(ActionObj);
	return AsyncActionTask;
}

void UGBWPHAsyncTaskBase::StartTask(UGBWPHAsyncActionObject* ActionObj)
{
	if (ActionObj && !ActionObj->Action_IsRunning())
	{
		(ActionObj->OnStart).BindUObject(this, &UGBWPHAsyncTaskBase::OnActionStart);
		(ActionObj->OnAbort).BindUObject(this, &UGBWPHAsyncTaskBase::OnActionAbort);
		(ActionObj->OnUpdate).BindUObject(this, &UGBWPHAsyncTaskBase::OnActionUpdate);
		(ActionObj->OnFinishd).BindUObject(this, &UGBWPHAsyncTaskBase::OnActionFinishd);
	}
} 

void UGBWPHAsyncTaskBase::OnActionStart(UGBWPHAsyncActionObject* ActionObj)
{
	OnStart.Broadcast(ActionObj);
}
void UGBWPHAsyncTaskBase::OnActionAbort(UGBWPHAsyncActionObject* ActionObj)
{
	OnAbort.Broadcast(ActionObj);
}
void UGBWPHAsyncTaskBase::OnActionUpdate(UGBWPHAsyncActionObject* ActionObj)
{
	OnUpdate.Broadcast(ActionObj);
}
void UGBWPHAsyncTaskBase::OnActionFinishd(UGBWPHAsyncActionObject* ActionObj)
{
	OnFinishd.Broadcast(ActionObj);
}




UGBWPHAsync_SetTimer::UGBWPHAsync_SetTimer()
{
	WorldContextObject = nullptr;
}


UGBWPHAsync_SetTimer::~UGBWPHAsync_SetTimer()
{
	
}

UGBWPHAsync_SetTimer* UGBWPHAsync_SetTimer::SetTimer(const UObject* WorldContextObject,
	float Time,
	bool bLooping,
	float InitialStartDelay /*= 0.f*/,
	float InitialStartDelayVariance /*= 0.f*/,
	UObject* ParameterObject/*= nullptr*/)
{
	if (!WorldContextObject)
	{
		FFrame::KismetExecutionMessage(TEXT("Invalid WorldContextObject. Cannot execute Set Timer."), ELogVerbosity::Error);
		return nullptr;
	}

	InitialStartDelay += FMath::RandRange(-InitialStartDelayVariance, InitialStartDelayVariance);
	if (Time <= 0.f || ((Time + InitialStartDelay) - InitialStartDelayVariance) < 0.f)
	{
		FFrame::KismetExecutionMessage(TEXT("SetTimer passed a negative or zero time.  The associated timer may fail to fire!  If using InitialStartDelayVariance, be sure it is smaller than (Time + InitialStartDelay)."), ELogVerbosity::Warning);
		return nullptr;
	}

	UGBWPHAsync_SetTimer* AsyncNode = NewObject<UGBWPHAsync_SetTimer>();
	AsyncNode->WorldContextObject = WorldContextObject;

	FTimerDynamicDelegate Delegate;
	Delegate.BindUFunction(AsyncNode, FName("CompletedEvent"));
	AsyncNode->TimerHandle = AsyncNode->SetTimerDelegate(Delegate, Time, bLooping, InitialStartDelay, InitialStartDelayVariance);

	AsyncNode->RegisterWithGameInstance((UObject*)WorldContextObject);

	FCoreUObjectDelegates::GetPreGarbageCollectDelegate().AddUObject(AsyncNode, &UGBWPHAsync_SetTimer::PreGarbageCollect);

	AsyncNode->ParameterObjectCache = ParameterObject;
	
	return AsyncNode;
}

FTimerHandle UGBWPHAsync_SetTimer::SetTimerDelegate(FTimerDynamicDelegate Delegate,
	float Time,
	bool bLooping,
	float InitialStartDelay /*= 0.f*/,
	float InitialStartDelayVariance /*= 0.f*/)
{
	FTimerHandle Handle;
	if (Delegate.IsBound())
	{
		World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull);
		if (World)
		{
			InitialStartDelay += FMath::RandRange(-InitialStartDelayVariance, InitialStartDelayVariance);
			if (Time <= 0.f || ((Time + InitialStartDelay) - InitialStartDelayVariance) < 0.f)
			{
				FFrame::KismetExecutionMessage(TEXT("SetTimer passed a negative or zero time.  The associated timer may fail to fire!  If using InitialStartDelayVariance, be sure it is smaller than (Time + InitialStartDelay)."), ELogVerbosity::Warning);
			}

			FTimerManager& TimerManager = World->GetTimerManager();
			Handle = TimerManager.K2_FindDynamicTimerHandle(Delegate);
			TimerManager.SetTimer(Handle, Delegate, Time, bLooping, (Time + InitialStartDelay));
		}
	}
	else
	{
		UE_LOG(LogBlueprintUserMessages, Warning,
			TEXT("SetTimer passed a bad function (%s) or object (%s)"),
			*Delegate.GetFunctionName().ToString(), *GetNameSafe(Delegate.GetUObject()));
	}

	return Handle;
}


void UGBWPHAsync_SetTimer::PreGarbageCollect()
{
	if (World)
	{
		FTimerManager& TimerManager = World->GetTimerManager();
		if (!TimerManager.TimerExists(TimerHandle))
		{
			SetReadyToDestroy();
		}
	}
}

void UGBWPHAsync_SetTimer::Activate()
{
	if (!WorldContextObject)
	{
		FFrame::KismetExecutionMessage(TEXT("Invalid WorldContextObject. Cannot execute Set Timer."), ELogVerbosity::Error);
		return;
	}

	Then.Broadcast(TimerHandle, ParameterObjectCache,Timer);
}


void UGBWPHAsync_SetTimer::CompletedEvent()
{
	bIsCompleted = true;
	if(!((UObject*)ParameterObjectCache)->IsValidLowLevel())
		ParameterObjectCache = nullptr;
	
	Completed.Broadcast(TimerHandle, ParameterObjectCache,Timer);
}

void UGBWPHAsync_SetTimer::Tick( float DeltaTime )
{
	if ( LastFrameNumberWeTicked == GFrameCounter )
		return;

	if(bIsCompleted)
		return;

	Timer = Timer + DeltaTime;
	Update.Broadcast(TimerHandle, ParameterObjectCache,Timer);
	
	LastFrameNumberWeTicked = GFrameCounter;
}

