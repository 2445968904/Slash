// Copyright GanBowen 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/KismetMathLibrary.h"
#include "UObject/NoExportTypes.h"
#include "GBWPowerfulHitNetWorkObject.generated.h"


class UGBWPH_ConditionGeneration;

UENUM(BlueprintType)
enum class EGBWPHTargetType :uint8
{
	Self UMETA(DisplayName = "Self"),
	Opponent UMETA(DisplayName = "Opponent"),
	Both  UMETA(DisplayName = "Both")
};

UENUM(BlueprintType)
enum class EGBWTrailWidthMode :uint8
{
	FromCentre UMETA(DisplayName = "From Centre"),
	FromFirst UMETA(DisplayName = "From First Socket"),
	FromSecond UMETA(DisplayName = "From Second Socket")
};

UENUM(BlueprintType)
enum class EGBWMoveType :uint8
{
	AwayFromOpponent UMETA(DisplayName = "AwayFromOpponent"),
	CloseToOppnent UMETA(DisplayName = "CloseToOppnent"),
	UP UMETA(DisplayName = "UP")
};

UENUM(BlueprintType)
enum class EGBWApplyType :uint8
{
	Overlap UMETA(DisplayName = "Overlap"),
	Accrual UMETA(DisplayName = "Accrual")
};

UENUM(BlueprintType)
enum class EGBWConditionSetEvaluationMode :uint8
{
	AllMatch UMETA(DisplayName = "AllMatch"),
	AnyMatch UMETA(DisplayName = "AnyMatch"),
	AllNotMatch UMETA(DisplayName = "AllNotMatch"),
	AnyNotMatch UMETA(DisplayName = "AnyNotMatch")
};

USTRUCT(BlueprintType, meta = (DisplayName = "PH_ConditionSet"))
struct FGBWPHConditionSet
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ConditionSet")
		EGBWConditionSetEvaluationMode EvaluationMode = EGBWConditionSetEvaluationMode::AllMatch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = "ConditionSet", meta = (MustImplement = GBWPHInterface_Condition))
		TMap<FString, UGBWPH_ConditionGeneration*> ConditionSet;
};

UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew, meta = (DisplayName = "GBW_PowerfulHitNetWorkObject"))
class GBWPOWERFULHIT_API UGBWPowerfulHitNetWorkObject : public UObject
{
	GENERATED_BODY()

	bool IsNameStableForNetworking() const override
	{
		return true;
	}
	bool IsSupportedForNetworking() const override
	{
		return true;
	}

protected:
	struct FWorldContext* WorldContext;

public:

	UGBWPowerfulHitNetWorkObject(){};

	// Allows the Object to use BP_Functions
	UFUNCTION(BlueprintCallable, Category="WorldContext")
		void SetWorldContext(UObject* NewWorldContext);

	UFUNCTION(BlueprintCallable, Category = "WorldContext")
		UObject* GetWorldContextObject();

	//~ Begin UObject Interface
	virtual class UWorld* GetWorld() const final;
	struct FWorldContext* GetWorldContext() const { return WorldContext; };
};

UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew, meta = (DisplayName = "GBWPH_ConditionGeneration_Base"))
class GBWPOWERFULHIT_API UGBWPH_ConditionGeneration : public UGBWPowerfulHitNetWorkObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "GBW|PH|Condition")
		bool GBW_GetConditionGenerationResult(AActor* SelfActor, AActor* OpponentActor) const;
	virtual bool GBW_GetConditionGenerationResult_Implementation(AActor* SelfActor, AActor* OpponentActor) const {
		return true;
	};

	UFUNCTION(BlueprintCallable, Category = "GBW|PH|Condition", meta = (DisplayName = "GetConditionResult"))
		bool GetConditionGenerationResult(AActor* SelfActor, AActor* OpponentActor) const {
		return GBW_GetConditionGenerationResult(SelfActor, OpponentActor);
	};
};

class UGBWPH_Feature_Base;
UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew, meta = (DisplayName = "GBWPH_FeatureConfig_Base"))
class GBWPOWERFULHIT_API UGBWPH_FeatureConfig_Base : public UGBWPowerfulHitNetWorkObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Target")
		EGBWPHTargetType TargetActor = EGBWPHTargetType::Self;

private:
	UPROPERTY(EditDefaultsOnly, Category = "InstanceMode")
		bool IsSingletonMode = false;

public:
	
	UFUNCTION(BlueprintPure, Category = "GBW|PH|Feature", meta = (DisplayName = "IsSingletonMode"))
		bool GetSingletonMode() {
		return IsSingletonMode;
	};

	UFUNCTION(BlueprintNativeEvent, Category = "GBW|PH|Feature")
		bool GBW_ExecuteFeature(AActor* OwnerActor, 
			AActor* SelfActor,
			AActor* OpponentActor, 
			FHitResult HitResult, 
			UGBWPH_Feature_Base* ExtantPHFExecutor, 
			UGBWPH_Feature_Base* &PHFExecutor);
	virtual bool GBW_ExecuteFeature_Implementation(AActor* OwnerActor, 
		AActor* SelfActor,
		AActor* OpponentActor, 
		FHitResult HitResult, 
		UGBWPH_Feature_Base* ExtantPHFExecutor, 
		UGBWPH_Feature_Base*& PHFExecutor) {
		return true;
	};

	UFUNCTION(BlueprintCallable, Category = "GBW|PH|Feature", meta = (DisplayName = "ExecuteFeature"))
		bool ExecuteFeature(AActor* OwnerActor, 
			AActor* SelfActor,
			AActor* OpponentActor, 
			FHitResult HitResult,
			UGBWPH_Feature_Base* ExtantPHFExecutor,
			UGBWPH_Feature_Base*& PHFExecutor) {
		return GBW_ExecuteFeature(OwnerActor,
			SelfActor,
			OpponentActor, 
			HitResult, 
			ExtantPHFExecutor,
			PHFExecutor);
	};
};

UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew, meta = (DisplayName = "GBWPH_Feature_Base"))
class GBWPOWERFULHIT_API UGBWPH_Feature_Base : public UGBWPowerfulHitNetWorkObject
{
	GENERATED_BODY()

private:
	UGBWPH_FeatureConfig_Base* ParamObject = nullptr;

public:

	UFUNCTION(BlueprintCallable, Category = "GBW|PH|Feature", meta = (DisplayName = "SetParamObject"))
		void SetParamObject(UGBWPH_FeatureConfig_Base* TheParamObject) {
		ParamObject = TheParamObject;
	};

	UFUNCTION(BlueprintPure, Category = "GBW|PH|Feature", meta = (DisplayName = "GetParamObject"))
		UGBWPH_FeatureConfig_Base* GetParamObject() {
		return ParamObject;
	};

	UFUNCTION(BlueprintNativeEvent, Category = "GBW|PH|Feature")
		bool GBW_ExecuteFeature(AActor* OwnerActor, 
			AActor* SelfActor,
			AActor* OpponentActor, 
			EGBWPHTargetType TargetActor, 
			FHitResult HitResult, 
			UGBWPH_FeatureConfig_Base* TheParamObject);
	virtual bool GBW_ExecuteFeature_Implementation(AActor* OwnerActor, 
		AActor* SelfActor,
		AActor* OpponentActor, 
		EGBWPHTargetType TargetActor, 
		FHitResult HitResult, 
		UGBWPH_FeatureConfig_Base* TheParamObject){
		return true;
	};

	UFUNCTION(BlueprintCallable, Category = "GBW|PH|Feature", meta = (DisplayName = "ExecuteFeature"))
		bool ExecuteFeature(AActor* OwnerActor, 
			AActor* SelfActor,
			AActor* OpponentActor, 
			EGBWPHTargetType TargetActor, 
			FHitResult HitResult, 
			UGBWPH_FeatureConfig_Base* TheParamObject){
		return GBW_ExecuteFeature(OwnerActor, SelfActor, OpponentActor, TargetActor, HitResult, TheParamObject);
	};

	UFUNCTION(BlueprintCallable, Category = "GBox|PH|Feature", meta = (DisplayName = "Destroy"))
		void Destroy()
	{
		if (this)
		{
			this->ConditionalBeginDestroy();
		}
	};
};

USTRUCT(BlueprintType, meta = (DisplayName = "GBW_PowerfulHitFeatureSet"))
struct FGBWPowerfulHitFeatureSet
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, Instanced, EditAnywhere, Category = "FeatureSet")
		TArray<UGBWPH_FeatureConfig_Base*> FeatureSet;

	FGBWPowerfulHitFeatureSet() {};
};