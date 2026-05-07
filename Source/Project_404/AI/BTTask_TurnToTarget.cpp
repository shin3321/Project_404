// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_TurnToTarget.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ABAI.h"
#include "Interface/FZFMonsterAIInterface.h"

UBTTask_TurnToTarget::UBTTask_TurnToTarget()
{
	NodeName = TEXT("TurnToTarget");
}

EBTNodeResult::Type UBTTask_TurnToTarget::ExecuteTask(
	UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	// 구현하고 싶은 것.
	// - 캐릭터를 향해 회전.
	// - 캐릭터 값과 태스크를 실행하는 AIPawn이 필요함.
	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!ControllingPawn)
	{
		return EBTNodeResult::Failed;
	}

	// 캐릭터(타겟).
	APawn* TargetPawn = Cast<APawn>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(BBKEY_TARGET));
	if (!TargetPawn)
	{
		return EBTNodeResult::Failed;
	}

	// - 두 객체를 사용해 회전 값을 구한 뒤 회전 처리.

	// 회전 속도를 얻어오기 위해 인터페이스로 형변환.
	IFZFMonsterAIInterface* AIPawn = Cast<IFZFMonsterAIInterface>(ControllingPawn);
	if (!AIPawn)
	{
		return EBTNodeResult::Failed;
	}

	// 회전 속도.
	float TurnSpeed = AIPawn->GetAITurnSpeed();

	// 바라봐야할 방향 - 타겟 위치에서 내 위치를 빼기.
	// => 타겟을 바라보는 방향 벡터가 구해짐.
	FVector LookVector = TargetPawn->GetActorLocation() - ControllingPawn->GetActorLocation();

	// 높이 성분 제거 (요(Yaw) 성분만 고려하기 위해).
	LookVector.Z = 0.0f;

	// 방향 벡터를 이용해 회전 각 구하기.
	FRotator TargetRot = FRotationMatrix::MakeFromX(LookVector).Rotator();

	// 부드럽게 회전 처리.

	// 이번 프레임에 설정할 회전 각도 구하기.
	FRotator FrameRot = FMath::RInterpTo(
		ControllingPawn->GetActorRotation(),
		TargetRot,
		GetWorld()->GetDeltaSeconds(),
		TurnSpeed
	);

	// 회전 설정.
	ControllingPawn->SetActorRotation(FrameRot);

	// 성공 반환.
	return EBTNodeResult::Succeeded;
}