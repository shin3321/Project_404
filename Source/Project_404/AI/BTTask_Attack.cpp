// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_Attack.h"
#include "Interface/FZFMonsterAIInterface.h"
#include "AIController.h"

UBTTask_Attack::UBTTask_Attack()
{
	// 노드 이름 설정.
	NodeName = TEXT("Attack");
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp,uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	// 구현하고 싶은 기능: 
	// Task에서 AI폰에 접근해서 공격하라고 명령 전달.
	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!ControllingPawn)
	{
		return EBTNodeResult::Failed;
	}

	// 인터페이스로 형변환(다운 캐스팅..).
	IFZFMonsterAIInterface* AIPawn = Cast<IFZFMonsterAIInterface>(ControllingPawn);
	if (!AIPawn)
	{
		return EBTNodeResult::Failed;
	}

	// 캐릭터가 공격을 종료할 때 실행될 델리게이트.
	FAICharacterAttackFinished OnAttackFinished;

	UE_LOG(LogTemp, Log, TEXT("[BTTask_Attack] 1. 공격 명령 전달 시작"));

	OnAttackFinished.BindLambda(
		[this, OwnerComp = &OwnerComp]()
		{
			UE_LOG(LogTemp, Log, TEXT("[BTTask_Attack] 3. 델리게이트 호출됨 -> 태스크 종료 처리"));
			if (OwnerComp)
			{
				FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
			}
		}
	);

	//// 람다 함수를 델리게이트에 바인딩.
	//OnAttackFinished.BindLambda(
	//	[this, &OwnerComp]()
	//	{
	//		// 태스크 끝내기.
	//		// 이 람다 본문이 실행되기 전에 
	//		// InProgress로 전달했기 때문.
	//		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	//	}
	//);

	// 캐릭터에 델리게이트 전달.
	AIPawn->SetAIAttackDelegate(OnAttackFinished);

	// AIPawn에 공격 명령 전달.
	AIPawn->AttackByAI();

	// !공격 명령 전달 후 곧바로 결과를 알 수 없음.
	// 따라서 진행중 상태를 전달.
	return EBTNodeResult::InProgress;
}