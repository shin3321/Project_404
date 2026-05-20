// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService_UpdateAttackPos.h"
#include "AIController.h"
#include "Interface/FZFMonsterAIInterface.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "FZFAI.h"
#include "DrawDebugHelpers.h"

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
		UE_LOG(LogTemp, Warning, TEXT("No ControllingPawn"));
		return;
	}

	// 플레이어 Target 구하기
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!Blackboard)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Blackboard"));
		return;
	}

	APawn* Target = Cast<APawn>(Blackboard->GetValueAsObject(BBKEY_TARGET));
	if (!Target)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Target"));
		return;
	}

	// 공격 사거리를 가져오기 위한 인터페이스 준비
	IFZFMonsterAIInterface* AIPawn = Cast<IFZFMonsterAIInterface>(ControllingPawn);
	if (!AIPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("No AIPawn Interface"));
		return;
	}

	// 벡터 구하기
	FVector MonsterLoc = ControllingPawn->GetActorLocation();
	FVector TargetLoc = Target->GetActorLocation();

	// 방향 벡터 구하기
	FVector DirFromTargetToMonster = MonsterLoc - TargetLoc;
	DirFromTargetToMonster.Z = 0.0f;
	DirFromTargetToMonster.Normalize();

	// 공격 사거리 불러오기
	float DesiredDistance = AIPawn->GetAIAttackRange();

	// AttackPos 설정
	FVector AttackPos = TargetLoc + DirFromTargetToMonster * DesiredDistance;

	/* 몬스터 공격 감지 범위 + 공격 사거리 시각 디버깅 코드 -> 지우지 마세요! */
	/*
	// 빨강 = 몬스터 위치(GetActorLocation)
	DrawDebugSphere(
		GetWorld(),
		MonsterLoc,
		40.f,
		12,
		FColor::Red,
		false,
		5.0f
	);

	// 초록 = 타겟 위치
	DrawDebugSphere(
		GetWorld(),
		TargetLoc,
		40.f,
		12,
		FColor::Green,
		false,
		5.0f
	);

	// 파랑 = 계산된 AttackPos
	DrawDebugSphere(
		GetWorld(),
		AttackPos,
		40.f,
		12,
		FColor::Blue,
		false,
		5.0f
	);

	// 선 연결
	DrawDebugLine(
		GetWorld(),
		MonsterLoc,
		TargetLoc,
		FColor::Yellow,
		false,
		5.0f,
		0,
		3.f
	);

	DrawDebugLine(
		GetWorld(),
		TargetLoc,
		AttackPos,
		FColor::Cyan,
		false,
		5.0f,
		0,
		3.f
	); 
	*/

	UE_LOG(LogTemp, Warning, TEXT("Desired: %.1f, CurrentDist: %.1f, AttackPos: %s"),
		DesiredDistance,
		FVector::Dist2D(MonsterLoc, TargetLoc),
		*AttackPos.ToString());

	// BB에 AttackPos 설정
	Blackboard->SetValueAsVector(BBKEY_ATTACKPOS, AttackPos);
}
