// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/FZFAIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BehaviorTree.h"
#include "AI/FZFAI.h"
#include "Interface/FZFMonsterAIInterface.h"

AFZFAIController::AFZFAIController()
{
	// 사용할 블랙보드 애셋 로드.
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BBAssetRef(
		TEXT("/Game/Project404/AI/BB_FZFMonster.BB_FZFMonster")
	);

	if (BBAssetRef.Succeeded())
	{
		BBAsset = BBAssetRef.Object;
	}
}

void AFZFAIController::RunAI()
{
	// 서버에서만 돌게 예외처리
	if (!HasAuthority())
	{
		return;
	}

	// 블랙보드 컴포넌트 포인터 가져오기.
	UBlackboardComponent* BlackboardPtr = Blackboard.Get();

	// 사용할 블랙보드 지정.
	// 두 번째 파라미터가 *& 타입이어서 명시적인 변수가 있어야 함.
	if (UseBlackboard(BBAsset, BlackboardPtr))
	{
		// 폰의 위치를 블랙보드에 저장.
		Blackboard->SetValueAsVector(
			BBKEY_HOMEPOS,
			GetPawn()->GetActorLocation()
		);

		// 비헤이비어 트리 할당하기.
		IFZFMonsterAIInterface* AIPawn = Cast<IFZFMonsterAIInterface>(GetPawn());
		if (!AIPawn)
		{
			return;
		}
		BTAsset = AIPawn->GetBT();


		// 비헤이비어 트리 실행.
		bool Result = RunBehaviorTree(BTAsset);

		// 예외 처리.
		ensureAlways(Result);
	}
}

void AFZFAIController::StopAI()
{
	StopMovement();

	UBehaviorTreeComponent* BTComponent = Cast<UBehaviorTreeComponent>(BrainComponent);

	if (BTComponent)
	{
		BTComponent->StopTree(EBTStopMode::Forced);
	}
}

void AFZFAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// 폰에 빙의하면 AI 로직 실행.
	// RunAI();
}
