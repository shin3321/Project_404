// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/BTDecorator_AttackInRange.h"
#include "Interface/FZFMonsterAIInterface.h"
#include "AIController.h"
#include "FZFAI.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"

UBTDecorator_AttackInRange::UBTDecorator_AttackInRange()
{
	NodeName = TEXT("AttackInRange");
}

bool UBTDecorator_AttackInRange::CalculateRawConditionValue(
	UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory
) const
{
	bool bResult
		= Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	// 구현하고 싶은 기능: 공격 범위 안에 있는지 판단.

	// 1. 폰 정보 가져오기.
	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!ControllingPawn)
	{
		return false;
	}

	// 인터페이스로 형변환.
	IFZFMonsterAIInterface* AIPawn = Cast<IFZFMonsterAIInterface>(ControllingPawn);
	if (!AIPawn)
	{
		return false;
	}

	// 2. 거리 계산(캐릭터와의 거리 계산).
	// 2.1 캐릭터 위치 가져오기 (블랙보드에 있음).
	APawn* Target = Cast<APawn>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(BBKEY_TARGET));
	if (!Target)
	{
		return false;
	}

	// 거리 계산. 3차원에서 2차원 거리로 변경(감지 이슈 관련)
	//float DistanceToTarget = ControllingPawn->GetDistanceTo(Target);
	float DistanceToTarget = FVector::Dist2D(ControllingPawn->GetActorLocation(), Target->GetActorLocation());

	// 계산한 결과가 공격 범위 안에 있는지 확인.
	float AttackDetectRange = AIPawn->GetAIAttackDetectRange();
	bResult = (DistanceToTarget <= AttackDetectRange);

	/* 감지 범위 확인 디버깅 코드 -> 지우지 마세요! */
	
	FVector MonsterLoc = ControllingPawn->GetActorLocation();
	FVector TargetLoc = Target->GetActorLocation();

	// 원통 변수
	float Radius = AttackDetectRange;
	float HalfHeight = 300.f; // 원통 높이 절반

	FVector Bottom = MonsterLoc - FVector(0, 0, HalfHeight);
	FVector Top = MonsterLoc + FVector(0, 0, HalfHeight);

	float DistanceToTarget1 = FVector::Dist2D(MonsterLoc, TargetLoc);

	return bResult;
}