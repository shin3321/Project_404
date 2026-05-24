#include "AI/Boss/BTTask_PhaseTransition.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Interface/FZFBossAIInterface.h"
#include "AI/Boss/FZFBossAI.h"

UBTTask_PhaseTransition::UBTTask_PhaseTransition()
{
	NodeName = TEXT("PhaseTransition");
}

EBTNodeResult::Type UBTTask_PhaseTransition::ExecuteTask(
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

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB)
	{
		return EBTNodeResult::Failed;
	}

	const int32 PrevPhase = BB->GetValueAsInt(BBKEY_CURRENTPHASE);
	const int32 NewPhase = FMath::Clamp(PrevPhase + 1, 1, 4);

	BB->SetValueAsInt(BBKEY_CURRENTPHASE, NewPhase);

	AIPawn->OnBossPhaseTransition(NewPhase);

	return EBTNodeResult::Succeeded;
}