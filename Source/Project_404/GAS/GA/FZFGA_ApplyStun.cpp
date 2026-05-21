// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA/FZFGA_ApplyStun.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayTag.h"
#include "AbilitySystemComponent.h"
#include "GameplayTag/FZFGameplayTags.h"

UFZFGA_ApplyStun::UFZFGA_ApplyStun()
{
	// 정책 설정
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
}

void UFZFGA_ApplyStun::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 캐릭터 이동 완전히 정지
	if (ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get()))
	{
		if (UCharacterMovementComponent* MovementComp = Character->GetCharacterMovement())
		{
			MovementComp->SetMovementMode(MOVE_None);
		}
	}

	// GE의 지속시간이 끝나서 태그가 제거될 때까지 대기하는 Task
	FGameplayTag StunTag = FZFGameplayTags::State_Debuff_Stun;
	UAbilityTask_WaitGameplayTagRemoved* WaitTagTask = UAbilityTask_WaitGameplayTagRemoved::WaitGameplayTagRemove(
		this,
		StunTag
	);

	if (WaitTagTask)
	{
		// 태그가 사라지면 OnStunTagRemoved 실행
		WaitTagTask->Removed.AddDynamic(this, &UFZFGA_ApplyStun::OnStunTagRemoved);
		WaitTagTask->ReadyForActivation();
	}
	else
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
	
}

void UFZFGA_ApplyStun::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// 스턴이 끝날 때 이동 기능 원상 복구
	if (ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get()))
	{
		if (UCharacterMovementComponent* MovementComp = Character->GetCharacterMovement())
		{
			// 걷기 상태로 되돌리기
			MovementComp->SetMovementMode(MOVE_Walking);
		}
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UFZFGA_ApplyStun::OnStunTagRemoved()
{
	// 3초 지났으므로 스턴 정상 종료
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
