// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA/FZFGA_BlackholeSlam.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystemComponent.h"
#include "Engine/World.h"
#include "Engine/OverlapResult.h"
#include "Character/Monster/FZFMonster.h"
#include "TimerManager.h"

UFZFGA_BlackholeSlam::UFZFGA_BlackholeSlam()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UFZFGA_BlackholeSlam::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UWorld* World = GetWorld();
	if (World)
	{
		// 0.1초 마다 Pulltargets 함수 반복 실행 (지속적인 당기기 연출)
		World->GetTimerManager().SetTimer(PullTimerHandle, this, &UFZFGA_BlackholeSlam::PullTargets, 0.1f, true);
		// PullDuration(2초) 뒤에 점프 페이즈로 넘어가기
		World->GetTimerManager().SetTimer(PhaseTimerHandle, this, &UFZFGA_BlackholeSlam::OnPullFinished, PullDuration, false);
	}
}


void UFZFGA_BlackholeSlam::PullTargets()
{
	ACharacter* AvatarCharacter = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!AvatarCharacter) return;

	FVector Center = AvatarCharacter->GetActorLocation();
	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(AvatarCharacter); // 자신은 탐색에서 제외

	// 주변 반경(SlamRadius) 탐색 (채널은 플레이어 폰으로 세팅)
	bool bHit = GetWorld()->OverlapMultiByChannel(OverlapResults, Center, FQuat::Identity, ECC_Pawn, FCollisionShape::MakeSphere(SlamRadius), Params);

	if (bHit)
	{
		for (auto& Result : OverlapResults)
		{
			ACharacter* Target = Cast<ACharacter>(Result.GetActor());
			if (Target && Target->GetCharacterMovement())
			{
				// 끌어당길 방향 (Z축은 0으로 만들어 하늘로 뜨지 않게 방지)
				FVector PullDirection = (Center - Target->GetActorLocation()).GetSafeNormal();
				PullDirection.Z = 0.0f;

				// 플레이어의 질량에 비례하여 쫀득하게 당기는 힘 부여
				Target->GetCharacterMovement()->AddForce(PullDirection * PullStrength * Target->GetCharacterMovement()->Mass);
			}
		}
	}
}

void UFZFGA_BlackholeSlam::OnPullFinished()
{
	// 더 이상 당기지 않도록 타이머 종료
	GetWorld()->GetTimerManager().ClearTimer(PullTimerHandle);

	ACharacter* AvatarCharacter = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!AvatarCharacter)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	// 몽타주 재생 (하늘로 도약)
	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		NAME_None,
		BlackholeSlamMontage,
		1.0f
	);

	if (MontageTask)
	{
		MontageTask->OnCompleted.AddDynamic(this, &UFZFGA_BlackholeSlam::OnMontageCompleted);
		MontageTask->OnInterrupted.AddDynamic(this, &UFZFGA_BlackholeSlam::OnMontageCompleted);
		MontageTask->OnCancelled.AddDynamic(this, &UFZFGA_BlackholeSlam::OnMontageCompleted);
		MontageTask->ReadyForActivation();
	}
}

void UFZFGA_BlackholeSlam::OnMontageCompleted()
{
	
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UFZFGA_BlackholeSlam::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(PullTimerHandle);
		World->GetTimerManager().ClearTimer(PhaseTimerHandle);
	}

	// AI 몬스터에게 스킬 액션이 완전히 종료되었음을 알림
	if (ActorInfo && ActorInfo->AvatarActor.IsValid())
	{
		if (AFZFMonster* Monster = Cast<AFZFMonster>(ActorInfo->AvatarActor.Get()))
		{
			Monster->NotifyAttackActionEnd();
		}
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}