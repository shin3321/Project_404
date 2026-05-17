// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA/FZFGA_AttackHitCheck.h"
#include "GAS/TA/FZFTA_SphereSweep.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Inventory/FZFHeldItemComponent.h"
#include "Item/FZFItemData.h"
#include "Character/Monster/FZFMonster.h"

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

	AActor* Avatar = GetAvatarActorFromActorInfo();

	if (!Avatar)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// TargetActor 생성 및 설정
	AFZFTA_SphereSweep* TargetActor = GetWorld()->SpawnActor<AFZFTA_SphereSweep>(TargetActorClass);
	if (TargetActor)
	{
		TargetActor->SourceActor = Avatar;
		TargetActor->bShowDebug = true; // 디버그 캡슐 활성화

		// 플레이어의 인벤토리(HeldItemComponent)에서 아이템 데이터 우선 확인
		if (UFZFHeldItemComponent* HeldItemComp = Avatar->FindComponentByClass<UFZFHeldItemComponent>())
		{
			if (UFZFItemData* ItemData = HeldItemComp->GetCurrentItemData())
			{
				TargetActor->bUseSocket = ItemData->bUseSocketTargeting;
				TargetActor->StartSocketName = ItemData->StartSocketName;
			}
		}
		// 플레이어 데이터가 없고 몬스터라면 몬스터 데이터 적용
		else if (AFZFMonster* Monster = Cast<AFZFMonster>(Avatar))
		{
			if (UFZFMonsterData* MData = Monster->GetMonsterData())
			{
				// MonsterData에 정의된 소켓 정보를 주입
				TargetActor->bUseSocket = MData->bUseSocketTargeting;
				TargetActor->StartSocketName = MData->AttackSocket;
			}
		}
	}

	// WaitTargetData 태스크 생성 및 실행
	// EGameplayTargetingConfirmation::Instant: 생성 즉시 판정을 수행
	UAbilityTask_WaitTargetData* WaitTargetDataTask = UAbilityTask_WaitTargetData::WaitTargetDataUsingActor(
		this,
		NAME_None,
		EGameplayTargetingConfirmation::Instant,
		TargetActor
	);
	// 콜백 함수 연결 (데이터가 유효할 때 호출)
	if (WaitTargetDataTask)
	{
		WaitTargetDataTask->ValidData.AddDynamic(this, &UFZFGA_AttackHitCheck::OnTargetDataReceived);

		// 태스크 활성화
		WaitTargetDataTask->ReadyForActivation();
	}

	if (TargetActor) 
	{
		// EGameplayTargetingConfirmation::Instant 관련
		TargetActor->ConfirmTargeting();
	}
}

void UFZFGA_AttackHitCheck::OnTargetDataReceived(const FGameplayAbilityTargetDataHandle& DataHandle)
{
	UE_LOG(LogTemp, Display, TEXT("EndAbility!!!!!!!!!!"));

	// TA가 브로드캐스트한 DataHandle이 여기로 들어옴
	if (DataHandle.Num() >0 && DataHandle.IsValid(0))
	{
		AActor* Avatar = GetAvatarActorFromActorInfo();
		if (Avatar)
		{
			TArray<TSubclassOf<UGameplayEffect>> EffectsToApply; // 적용할 효과 클래스담는 배열

			// 플레이어의 인벤토리(HeldItemComponent)에서 아이템 데이터의 GE 확인
			if(UFZFHeldItemComponent* HeldItemComp = Avatar->FindComponentByClass<UFZFHeldItemComponent>())
			{
				if (UFZFItemData* ItemData = HeldItemComp->GetCurrentItemData())
				{
					EffectsToApply = ItemData->AllowedEffectClasses;
				}
			}
			// 플레이어 데이터가 없고 몬스터라면 몬스터 데이터의 GE 확인
			else if (AFZFMonster* Monster = Cast<AFZFMonster>(Avatar))
			{
				if (UFZFMonsterData* MData = Monster->GetMonsterData())
				{
					EffectsToApply = MData->AllowedEffectClasses;
				}
			}
			for(const TSubclassOf<UGameplayEffect>& EffectClass : EffectsToApply)
			{
				if (EffectClass)
				{
					ApplyGameplayEffectToTarget(
						CurrentSpecHandle,      // 어빌리티 스펙 핸들
						CurrentActorInfo,       // 액터 정보 포인터
						CurrentActivationInfo,  // 활성화 정보
						DataHandle,             // 전송할 타겟 데이터 핸들
						EffectClass,          // GameplayEffect 클래스
						GetAbilityLevel()		// GameplayEffect 레벨
					);
				}
			}

		}		
	}

	// 모든 로직이 완료되면 능력을 종료
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
