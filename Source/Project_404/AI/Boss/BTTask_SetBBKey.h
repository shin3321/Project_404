#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType.h"
#include "AI/Boss/FZFBossState.h"
#include "BTTask_SetBBKey.generated.h"

UENUM(BlueprintType)
enum class ESetBBValueType : uint8
{
	Bool,
	Int,
	Float,
	Enum_BossState,
	Vector,
	Object,
	Clear
};

UCLASS()
class PROJECT_404_API UBTTask_SetBBKey : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_SetBBKey();

protected:
	virtual EBTNodeResult::Type ExecuteTask(
		UBehaviorTreeComponent& OwnerComp,
		uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector BlackboardKey;

	UPROPERTY(EditAnywhere, Category = "Value")
	ESetBBValueType ValueType = ESetBBValueType::Bool;

	UPROPERTY(EditAnywhere, Category = "Value", meta = (EditCondition = "ValueType == ESetBBValueType::Bool"))
	bool BoolValue = false;

	UPROPERTY(EditAnywhere, Category = "Value", meta = (EditCondition = "ValueType == ESetBBValueType::Int"))
	int32 IntValue = 0;

	UPROPERTY(EditAnywhere, Category = "Value", meta = (EditCondition = "ValueType == ESetBBValueType::Float"))
	float FloatValue = 0.0f;

	// 컴포짓 들어왔던 기존 상태
	UPROPERTY(EditAnywhere, Category = "Value", meta = (EditCondition = "ValueType == ESetBBValueType::Enum_BossState"))
	EBossState ExpectedState = EBossState::Waiting;

	// 다음 전이 원하는 상태
	UPROPERTY(EditAnywhere, Category = "Value", meta = (EditCondition = "ValueType == ESetBBValueType::Enum_BossState"))
	EBossState NextState = EBossState::Active;

	UPROPERTY(EditAnywhere, Category = "Value", meta = (EditCondition = "ValueType == ESetBBValueType::Vector"))
	FVector VectorValue = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Category = "Value", meta = (EditCondition = "ValueType == ESetBBValueType::Object"))
	TObjectPtr<AActor> ObjectValue = nullptr;
};