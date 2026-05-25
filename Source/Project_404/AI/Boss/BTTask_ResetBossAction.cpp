#include "AI/Boss/BTTask_ResetBossAction.h"
#include "AIController.h"
#include "Interface/FZFBossAIInterface.h"

UBTTask_ResetBossAction::UBTTask_ResetBossAction()
{
	NodeName = TEXT("ResetBossAction");
}

EBTNodeResult::Type UBTTask_ResetBossAction::ExecuteTask(
	UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory
)
{
	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!ControllingPawn)
	{
		return EBTNodeResult::Failed;
	}

	IFZFBossAIInterface* AIPawn = Cast<IFZFBossAIInterface>(ControllingPawn);
	if (!AIPawn)
	{
		return EBTNodeResult::Failed;
	}

	AIPawn->ResetBossAction();

	return EBTNodeResult::Succeeded;
}