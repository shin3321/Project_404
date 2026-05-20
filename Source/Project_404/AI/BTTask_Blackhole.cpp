// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/BTTask_Blackhole.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "Engine/OverlapResult.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GAS/Attributes/FZFMonsterSet.h"
#include "Physics/FZFCollision.h"

UBTTask_Blackhole::UBTTask_Blackhole()
{
	// 노드 이름 설정
	NodeName = TEXT("Blackhole");

	// 매 프레임 당기기 연산을 위해
	bNotifyTick = true;

	// 맴버 변수(ElapsedTime)를 독립적으로 안전하게 사용하기 위해
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTTask_Blackhole::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	// 구현하고 싶은 기능
	// Task에서 블랙홀을 생성해서 플레이어를 끌어당기는 기능
	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!ControllingPawn)
	{
		return EBTNodeResult::Failed;
	}

	// ASC 가져오기
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(ControllingPawn);
	if (ASC)
	{
		PullRange = ASC->GetNumericAttribute(UFZFMonsterSet::GetDetectRangeAttribute());
	
		PullDuration = ASC->GetNumericAttribute(UFZFMonsterSet::GetAttackSpeedAttribute());
		PullStrength = ASC->GetNumericAttribute(UFZFMonsterSet::GetPullStrengthAttribute());
	}
	else
	{
		// ASC를 찾지 못했을 경우 기본값
		PullDuration = 2.0f;
		PullRange = 500.0f;
		PullStrength = 1000.0f;
	}

	// 타이머 초기화
	ElapsedTime = 0.0f;

	// 태스크를 즉시 종료하지 않고 TickTask가 돌도록 InProgress 반환
	return EBTNodeResult::InProgress;
}

void UBTTask_Blackhole::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	// 시간 체크
	ElapsedTime += DeltaSeconds;
	if (ElapsedTime >= PullDuration) // ASC에서 가져온 값을 기준
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	// 당기기 로직 실행
	APawn* Monster = OwnerComp.GetAIOwner()->GetPawn();
	if (!Monster)
	{
		return;
	}

	FVector Center = Monster->GetActorLocation();
	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Monster);

	// 주변 플레이어 탐색
	bool bHit = GetWorld()->OverlapMultiByChannel(OverlapResults, Center, FQuat::Identity, CCHANNEL_FZFPLAYER, FCollisionShape::MakeSphere(PullRange), Params);

	if (bHit)
	{
		for (auto& Result : OverlapResults)
		{
			ACharacter* Target = Cast<ACharacter>(Result.GetActor());
			if (Target && Target->GetCharacterMovement())
			{
				FVector PullDirection = (Center - Target->GetActorLocation()).GetSafeNormal();
				PullDirection.Z = 0.0f;

				// 플레이어에게 지속적으로 끌어당기는 물리력 적용
				Target->GetCharacterMovement()->AddForce(PullDirection * PullStrength * Target->GetCharacterMovement()->Mass);
			}
		}
	}
}
