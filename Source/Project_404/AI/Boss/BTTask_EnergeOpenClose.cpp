// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Boss/BTTask_EnergeOpenClose.h"
#include "Interface/FZFBossAIInterface.h"
#include "AIController.h"

UBTTask_EnergeOpenClose::UBTTask_EnergeOpenClose()
{
	NodeName = TEXT("EnergyRelay");
}

EBTNodeResult::Type UBTTask_EnergeOpenClose::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

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

	if (bIsOnEnergy)
	{
		AIPawn->NotifyWaitingStarted();
	}
	else
	{
		AIPawn->NotifyWaitingEnded();
	}

	return EBTNodeResult::Succeeded;

}