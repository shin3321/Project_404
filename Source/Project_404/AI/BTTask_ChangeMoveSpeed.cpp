// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_ChangeMoveSpeed.h"
#include "AIController.h"
#include "Interface/FZFMonsterAIInterface.h"

UBTTask_ChangeMoveSpeed::UBTTask_ChangeMoveSpeed()
{
	NodeName = TEXT("ChangeMoveSpeed");

}

EBTNodeResult::Type UBTTask_ChangeMoveSpeed::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!ControllingPawn)
	{
		return EBTNodeResult::Failed;;
	}

	IFZFMonsterAIInterface* AIPawn = Cast<IFZFMonsterAIInterface>(ControllingPawn);
	if (!AIPawn)
	{
		return EBTNodeResult::Failed;;
	}

	AIPawn->SetAIMoveSpeedMode(MoveSpeedMode);

	return EBTNodeResult::Succeeded;
}
