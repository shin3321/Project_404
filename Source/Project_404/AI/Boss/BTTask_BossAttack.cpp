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

	//// 더미 동력원 파괴 테스트
	//TArray<AActor*> FoundRelays;

	//UGameplayStatics::GetAllActorsOfClass(
	//	GetWorld(),
	//	AFZFEnergyRelay::StaticClass(),
	//	FoundRelays
	//);

	//for (AActor* Actor : FoundRelays)
	//{
	//	AFZFEnergyRelay* Relay = Cast<AFZFEnergyRelay>(Actor);
	//	if (!Relay)
	//	{
	//		continue;
	//	}

	//	if (Relay->IsDead())
	//	{
	//		continue;
	//	}

	//	Relay->HandleDead();
	//	break;
	//}
	//// 여기까지

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