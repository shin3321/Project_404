// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Boss/BTTask_BossAttack.h"
#include "Interface/FZFBossAIInterface.h"
#include "AIController.h"
#include "Character/Monster/MonsterData/FZFBossData.h"
#include "Boss/FZFEnergyRelay.h"
#include "Kismet/GameplayStatics.h"

UBTTask_BossAttack::UBTTask_BossAttack() 
{
	NodeName = TEXT("BossAttack");
}

EBTNodeResult::Type UBTTask_BossAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	FBossAICharacterAttackFinished OnAttackFinished;

	OnAttackFinished.BindLambda(
		[this, &OwnerComp]()
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
	);

	AIPawn->SetAIAttackDelegate(OnAttackFinished);

	AIPawn->AttackByAI();

	return EBTNodeResult::InProgress;
}