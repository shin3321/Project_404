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
	
	// 공중 도약 몽타주 재생
	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		NAME_None,
		BlackholeSlamMontage,
		1.5f
	);

	if (MontageTask)
	{
		// 몽타주 상태에 따른 델리게이트 바인딩
		MontageTask->OnCompleted.AddDynamic(this, &UFZFGA_BlackholeSlam::OnMontageCompleted);
		MontageTask->OnInterrupted.AddDynamic(this, &UFZFGA_BlackholeSlam::OnMontageCancelled);
		MontageTask->OnCancelled.AddDynamic(this, &UFZFGA_BlackholeSlam::OnMontageCancelled);

		// 태스크 활성화
		MontageTask->ReadyForActivation();
	}
	else
	{
		// 태스크 생성 실패 시 안전하게 어빌리티 종료
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
}

void UFZFGA_BlackholeSlam::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UFZFGA_BlackholeSlam::OnMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UFZFGA_BlackholeSlam::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
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