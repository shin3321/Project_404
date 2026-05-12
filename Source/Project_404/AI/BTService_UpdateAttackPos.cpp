// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService_UpdateAttackPos.h"
#include "AIController.h"
#include "Interface/FZFMonsterAIInterface.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "FZFAI.h"

UBTService_UpdateAttackPos::UBTService_UpdateAttackPos()
{
	NodeName = TEXT("UpdateAPos");

	Interval = 0.1;
}

void UBTService_UpdateAttackPos::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// 몬스터 자신 pawn 구하기
	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!ControllingPawn)
	{
		return;
	}

	// 플레이어 Target 구하기
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!Blackboard)
	{
		return;
	}

	APawn* Target = Cast<APawn>(Blackboard->GetValueAsObject(BBKEY_ATTACKPOS));
	if (!Target)
	{
		return;
	}

	// 공격 사거리를 가져오기 위한 인터페이스 준비
	IFZFMonsterAIInterface* AIPawn = Cast<IFZFMonsterAIInterface>(ControllingPawn);
	if (!AIPawn)
	{
		return;
	}

	FVector MonsterLoc = ControllingPawn->GetActorLocation();
	FVector TargetLoc = Target->GetActorLocation();

	FVector DirFromTargetToMonster = MonsterLoc - TargetLoc;
	DirFromTargetToMonster.Z = 0.0f;
	DirFromTargetToMonster.Normalize();

	float DesiredDistance = AIPawn




}
