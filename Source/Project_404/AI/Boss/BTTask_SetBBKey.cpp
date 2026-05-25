#include "AI/Boss/BTTask_SetBBKey.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTTask_SetBBKey::UBTTask_SetBBKey()
{
	NodeName = TEXT("Set BB Key");
}

EBTNodeResult::Type UBTTask_SetBBKey::ExecuteTask(
	UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory)
{
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB)
	{
		return EBTNodeResult::Failed;
	}

	const FName KeyName = BlackboardKey.SelectedKeyName;
	if (KeyName == NAME_None)
	{
		return EBTNodeResult::Failed;
	}

	switch (ValueType)
	{
	case ESetBBValueType::Bool:
		BB->SetValueAsBool(KeyName, BoolValue);
		break;

	case ESetBBValueType::Int:
		BB->SetValueAsInt(KeyName, IntValue);
		break;

	case ESetBBValueType::Float:
		BB->SetValueAsFloat(KeyName, FloatValue);
		break;

	case ESetBBValueType::Enum_BossState:
		BB->SetValueAsEnum(KeyName, static_cast<uint8>(BossStateValue));
		break;

	case ESetBBValueType::Vector:
		BB->SetValueAsVector(KeyName, VectorValue);
		break;

	case ESetBBValueType::Object:
		BB->SetValueAsObject(KeyName, ObjectValue);
		break;

	case ESetBBValueType::Clear:
		BB->ClearValue(KeyName);
		break;

	default:
		return EBTNodeResult::Failed;
	}

	return EBTNodeResult::Succeeded;
}