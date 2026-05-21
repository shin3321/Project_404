// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA/FZFGA_AttackHitCheck.h"
#include "GAS/TA/FZFTA_Base.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Inventory/FZFHeldItemComponent.h"
#include "Item/FZFItemData.h"
#include "Character/Monster/FZFMonster.h"
#include "GAS/FZFAbilitySystemComponent.h"

UFZFGA_AttackHitCheck::UFZFGA_AttackHitCheck()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
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
	AFZFTA_Base* TargetActor = GetWorld()->SpawnActor<AFZFTA_Base>(TargetActorClass);
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

			// 능력 사용자의 ASC(Ability System Component) 가져오기
			UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
			if (ASC)
			{
				for (const TSubclassOf<UGameplayEffect>& EffectClass : EffectsToApply)
				{
					if (EffectClass)
					{
						// 이펙트 컨텍스트 생성 및 가해자(Instigator) 명시
						// AvatarActor를 가해자로 지정하여 캐릭터의 AttributeSet 스탯을 정상적으로 캡처하게 만듭니다.
						FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
						EffectContext.AddInstigator(Avatar, Avatar);

						// 수동으로 컨텍스트를 주입한 안전한 이펙트 스펙(Spec) 생성
						FGameplayEffectSpecHandle NewSpecHandle = ASC->MakeOutgoingSpec(EffectClass, GetAbilityLevel(), EffectContext);
						if (NewSpecHandle.IsValid())
						{
							// 타겟 데이터 핸들(DataHandle) 내에 있는 대상들에게 스펙 적용
							ApplyGameplayEffectSpecToTarget(
								CurrentSpecHandle,      // 어빌리티 스펙 핸들
								CurrentActorInfo,       // 액터 정보 포인터
								CurrentActivationInfo,  // 활성화 정보
								NewSpecHandle,          // 수동으로 생성한 GameplayEffect 스펙 핸들
								DataHandle              // 전송할 타겟 데이터 핸들
							);
						}
					}
				}
			}
		}		
	}

	// 모든 로직이 완료되면 능력을 종료
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
