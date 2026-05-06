// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_FindPatrolPath.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ABAI.h"
#include "NavigationSystem.h"
#include "AIController.h"
#include "Interface/FZFMonsterAIInterface.h"

UBTTask_FindPatrolPath::UBTTask_FindPatrolPath()
{
    // 노드 이름 설정.
    NodeName = TEXT("FindPatrolPos");
}

EBTNodeResult::Type UBTTask_FindPatrolPath::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

    // 구현 기능: 다음에 정찰할 위치 선택.
    // 1. 블랙보드의 HomePos위치를 기준으로 일정 반경 안에서 랜덤 위치 선택
    FVector Origin = OwnerComp.GetBlackboardComponent()->GetValueAsVector(
        BBKEY_HOMEPOS
    );
    
    
    // BT 시스템에서 제어하는 폰 정보 가져오기.
    // 비헤이비어트리 컴포넌트의 OwnerComp는 AIController
    APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
    if (!ControllingPawn)
    {
        return EBTNodeResult::Failed;
    }

    UWorld* World = ControllingPawn->GetWorld();
    if (!World)
    {
        return EBTNodeResult::Failed;
    }

    // 1.1 랜덤 위치 선택 시 네비게이션 기능 활용.
    UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(World);
    if (!NavSystem)
    {
        return EBTNodeResult::Failed;
    }

    // 인터페이스로 형변환.
    IFZFMonsterAIInterface* AIPawn = Cast<IFZFMonsterAIInterface>(ControllingPawn);
    if (!AIPawn)
    {
        return EBTNodeResult::Failed;
    }

    // 정찰 반경.
    float PatrolRadius = AIPawn->GetAIPatrolRadius();

    // 랜덤으로 선택된 위치를 저장할 변수.
    FNavLocation NextPatrolPos;
    bool bResult = NavSystem->GetRandomPointInNavigableRadius(
        Origin,
        PatrolRadius,
        NextPatrolPos
    );


    // 2. 선택된 위치를 블랙보드의 PatrolPos에 저장.
    if (bResult)
    {
        OwnerComp.GetBlackboardComponent()->SetValueAsVector(
            BBKEY_PATROLPOS,
            NextPatrolPos
        );

        // 성공 반환.
        return EBTNodeResult::Succeeded;
    }
    
    return EBTNodeResult::Failed;
}
