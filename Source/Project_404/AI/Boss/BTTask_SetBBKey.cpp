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
	{
		const EBossState CurrentState = static_cast<EBossState>(BB->GetValueAsEnum(KeyName));

		UE_LOG(LogTemp, Warning,
			TEXT("[SetBBKey] Current=%d Expected=%d Next=%d Key=%s"),
			static_cast<uint8>(CurrentState),
			static_cast<uint8>(ExpectedState),
			static_cast<uint8>(NextState),
			*KeyName.ToString()
		);

		if (CurrentState != ExpectedState)
		{
			UE_LOG(LogTemp, Warning,
				TEXT("[SetBBKey] BossState already changed. Current=%d Expected=%d. Skip Set NextState=%d"),
				static_cast<uint8>(CurrentState),
				static_cast<uint8>(ExpectedState),
				static_cast<uint8>(NextState)
			);

			return EBTNodeResult::Succeeded;
		}

		BB->SetValueAsEnum(KeyName, static_cast<uint8>(NextState));

		const EBossState AfterState =
			static_cast<EBossState>(BB->GetValueAsEnum(KeyName));

		UE_LOG(LogTemp, Warning,
			TEXT("[SetBBKey] After Set BossState=%d"),
			static_cast<uint8>(AfterState)
		);

		break;
	}

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