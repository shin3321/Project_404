// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA/FZFGA_AttackHitCheck.h"
#include "GAS/TA/FZFTA_LineTrace.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"

UFZFGA_AttackHitCheck::UFZFGA_AttackHitCheck()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UFZFGA_AttackHitCheck::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!TargetActorClass)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// TargetActor 생성 및 설정
	AFZFTA_LineTrace* TargetActor = GetWorld()->SpawnActor<AFZFTA_LineTrace>(TargetActorClass);
	if (TargetActor)
	{
		TargetActor->bShowDebug = true; // 디버그 캡슐 활성화
	}

	// WaitTargetData 태스크 생성 및 실행
	// EGameplayTargetingConfirmation::Instant: 생성 즉시 판정을 수행
	UAbilityTask_WaitTargetData* WaitTargetDataTask = UAbilityTask_WaitTargetData::WaitTargetData(
		this,
		NAME_None,
		EGameplayTargetingConfirmation::Instant,
		TargetActorClass
	);
	// 콜백 함수 연결 (데이터가 유효할 때 호출)
	WaitTargetDataTask->ValidData.AddDynamic(this, &UFZFGA_AttackHitCheck::OnTargetDataReceived);

	// 태스크 활성화
	WaitTargetDataTask->ReadyForActivation();

}

void UFZFGA_AttackHitCheck::OnTargetDataReceived(const FGameplayAbilityTargetDataHandle& DataHandle)
{
	// TA가 브로드캐스트한 DataHandle이 여기로 들어옴
	if (DataHandle.IsValid(0))
	{
		// 여기서 적에게 데미지(GE)를 적용합니다.
		// ApplyGameplayEffectToTarget 등의 로직이 이곳에 위치하게 됩니다.
		UE_LOG(LogTemp, Log, TEXT("Target Data Received!"));
	}

	// 모든 로직이 완료되면 능력을 종료합니다.
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
