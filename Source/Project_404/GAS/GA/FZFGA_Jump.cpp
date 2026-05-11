// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA/FZFGA_Jump.h"
#include "Character/Player/FZFCharacterPlayer.h"

UFZFGA_Jump::UFZFGA_Jump()
{
	// 인스턴싱 정책: 어빌리티가 실행될 때마다 인스턴스를 생성 (데이터 관리가 편함)
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	// 실행 정책 (반응성 강화)
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

}

void UFZFGA_Jump::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    AFZFCharacterPlayer* Character = Cast<AFZFCharacterPlayer>(ActorInfo->AvatarActor.Get());
    if (Character)
    {
        Character->Jump(); // 물리 점프 실행
    }

    // 즉시 종료함
    bool bReplicateEndAbility = true;
    bool bWasCancelled = false;
    EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UFZFGA_Jump::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
