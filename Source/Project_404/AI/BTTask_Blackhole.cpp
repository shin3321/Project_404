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
#include "Character/Monster/FZFMonster.h"
#include "Physics/FZFCollision.h"
#include "GameplayTag/FZFGameplayTags.h"

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
	UE_LOG(LogTemp, Warning, TEXT("[Blackhole] 태스크 진입 성공!"));
	Super::ExecuteTask(OwnerComp, NodeMemory);

	// 구현하고 싶은 기능
	// Task에서 블랙홀을 생성해서 플레이어를 끌어당기는 기능
	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!ControllingPawn)
	{
		return EBTNodeResult::Failed;
	}
	// 타이머 초기화
	ElapsedTime = 0.0f;
	// 명단 초기화
	ActiveGEHandles.Empty();
	// ASC 가져오기
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(ControllingPawn);
	if (ASC)
	{
		PullRange = ASC->GetNumericAttribute(UFZFMonsterSet::GetDetectRangeAttribute());
	
		PullDuration = ASC->GetNumericAttribute(UFZFMonsterSet::GetAttackSpeedAttribute());
		PullStrength = ASC->GetNumericAttribute(UFZFMonsterSet::GetPullStrengthAttribute());
	}


	// 태스크를 즉시 종료하지 않고 TickTask가 돌도록 InProgress 반환
	return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UBTTask_Blackhole::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UE_LOG(LogTemp, Error, TEXT("[Blackhole] 태스크 강제 종료됨! (Abort) -> 청소 로직 실행"));
	CleanupAllBlackholeEffects();
	return Super::AbortTask(OwnerComp, NodeMemory);
}

void UBTTask_Blackhole::CleanupAllBlackholeEffects()
{
	for (auto& Pair : ActiveGEHandles)
	{
		if (Pair.Key.IsValid())
		{
			ACharacter* Target = Pair.Key.Get();
			UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target);

			if (TargetASC && BlackholeGEClass)
			{
				// 1. 핸들(영수증)로 먼저 정상 삭제 시도
				if (Pair.Value.IsValid())
				{
					TargetASC->RemoveActiveGameplayEffect(Pair.Value);
				}

				// 2. [무적의 강제 삭제] 핸들이 끊겼거나 찌꺼기가 남았을 경우를 대비해
				// 타겟의 몸에 있는 'BlackholeGEClass' 디버프를 무조건 싹 다 지워버림 (-1)
				TargetASC->RemoveActiveGameplayEffectBySourceEffect(BlackholeGEClass, nullptr, -1);

				UE_LOG(LogTemp, Warning, TEXT("[Blackhole] 최종 청소 - 플레이어(%s) 디버프 강제 삭제 완료!"), *Target->GetName());
			}
		}
	}
	ActiveGEHandles.Empty();
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

	// 이번 프레임에 범위 안에서 새롭게 감지된 타겟들을 기록할 임시 세트
	TSet<TWeakObjectPtr<ACharacter>> CurrentDetectedTargets;

	// 주변 플레이어 탐색
	bool bHit = GetWorld()->OverlapMultiByChannel(OverlapResults, Center, FQuat::Identity, CCHANNEL_FZFPLAYER, FCollisionShape::MakeSphere(PullRange), Params);

	if (bHit)
	{
		for (auto& Result : OverlapResults)
		{
			AActor* OverlappedActor = Result.GetActor();
			if (!OverlappedActor)
			{
				continue;
			}

			if (OverlappedActor->IsA(AFZFMonster::StaticClass()))
			{
				continue;
			}

			ACharacter* Target = Cast<ACharacter>(OverlappedActor);
			if (Target && Target->GetCharacterMovement())
			{
				// 이번 프레임 감지 목록에 등록
				CurrentDetectedTargets.Add(Target);

				// 저항 연출 및 탈출 로직
				UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target);
				if (TargetASC && BlackholeGEClass)
				{
					// 명단에 없는 새로운 유저 발견 -> 블랙홀 디버프 GameplayEffect 부여
					if (!ActiveGEHandles.Contains(Target))
					{
						FGameplayEffectContextHandle EffectContext = TargetASC->MakeEffectContext();
						EffectContext.AddInstigator(Monster, Monster);

						FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(BlackholeGEClass, 1.0f, EffectContext);
						if (SpecHandle.IsValid())
						{
							// 플레이어에게 영구 디버프(Infinite)를 부여하고, 그 Handle을 보관
							FActiveGameplayEffectHandle ActiveHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
							ActiveGEHandles.Add(Target, ActiveHandle);
						}
					}

					// 이동 제어 및 줄다리기
					FVector PullDirection = (Center - Target->GetActorLocation()).GetSafeNormal();
					PullDirection.Z = 0.0f;

					// 기본 흡입 속도 벡터 계산 (가만히 서 있을 때 적용할 기본 이동량)
					FVector HorizontalPullVelocity = PullDirection * PullStrength;

					FGameplayTag SpeedBuffTag = FZFGameplayTags::State_Movement_Run;
					if (TargetASC->HasMatchingGameplayTag(SpeedBuffTag))
					{
						// 유저가 현재 WASD 키를 누르고 있는 입력 방향 벡터 가져오기
						FVector PlayerInput = Target->GetLastMovementInputVector();

						// 유저가 이동 입력을 누르고 있고, 도망치는 중이라면 가중치를 줄여서 줄다리기를 성립시킴
						if (!PlayerInput.IsNearlyZero() && FVector::DotProduct(PlayerInput, PullDirection) < 0.0f)
						{
							// GE에 의해 감속된 플레이어의 실제 달리기 최대 속도 벡터 (방향 * 감속된 MaxWalkSpeed)
							FVector PlayerRunVelocity = PlayerInput.GetSafeNormal() * Target->GetCharacterMovement()->MaxWalkSpeed;
							HorizontalPullVelocity = PlayerRunVelocity + (PullDirection * PullStrength);
						}
					}

					Target->GetCharacterMovement()->Velocity = FVector(HorizontalPullVelocity.X, HorizontalPullVelocity.Y, Target->GetCharacterMovement()->Velocity.Z);
				}
			}
		}
	}

	// 실시간 개별 탈출자 처리
	// 지난 프레임에는 있었는데 이번 프레임 감지목록에 없다 -> 범위 밖으로 탈출
	for (auto It = ActiveGEHandles.CreateIterator(); It; ++It)
	{
		if (It->Key.IsValid())
		{
			ACharacter* ActiveTarget = It->Key.Get();
			if (!CurrentDetectedTargets.Contains(ActiveTarget))
			{
				// Target의 ASC 가져옴
				UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(ActiveTarget);

				if (TargetASC && BlackholeGEClass)
				{
					// 핸들로 정상 삭제 시도
					if (It->Value.IsValid())
					{
						TargetASC->RemoveActiveGameplayEffect(It->Value);
					}

					// 무적의 강제 삭제 범위 밖으로 나간 즉시 해당 클래스 GE 강제 말살
					TargetASC->RemoveActiveGameplayEffectBySourceEffect(BlackholeGEClass, nullptr, -1);
				}
				It.RemoveCurrent(); // 명단 제외
			}
		}
		else
		{
			// 대상 유저가 도중에 소멸한 경우 안전하게 제거
			It.RemoveCurrent();
		}
	}
}

void UBTTask_Blackhole::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	UE_LOG(LogTemp, Warning, TEXT("[Blackhole] 태스크 정상 종료됨! -> 청소 로직 실행"));
	CleanupAllBlackholeEffects();
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}
