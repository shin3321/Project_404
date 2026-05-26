// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA/FZFGA_AttackHitCheck.h"
#include "GAS/TA/FZFTA_Base.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Inventory/FZFHeldItemComponent.h"
#include "Item/FZFItemData.h"
#include "Character/Monster/FZFMonster.h"
#include "GAS/FZFAbilitySystemComponent.h"
#include "GameplayTag/FZFGameplayTags.h"
#include "FZFGA_AttackHitCheck_Channeling.h"

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
		WaitTargetDataTask->Cancelled.AddDynamic(this, &UFZFGA_AttackHitCheck::OnTargetDataCancelled);
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
	if (DataHandle.Num() > 0 && DataHandle.IsValid(0))
	{
		AActor* Avatar = GetAvatarActorFromActorInfo();
		UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();

		if (Avatar && ASC)
		{
			// 현재 장착된 아이템 데이터 가져오기
			UFZFItemData* CurrentItemData = nullptr;
			UFZFHeldItemComponent* HeldItemComp = Avatar->FindComponentByClass<UFZFHeldItemComponent>(); // 태그 추출을 위해 바깥으로 변수 선언 통합
			if (HeldItemComp)
			{
				CurrentItemData = HeldItemComp->GetCurrentItemData();
			}

			// 서버에서만 GameplayCue 실행 (모든 클라이언트에 복제됨)
			if (HasAuthority(&CurrentActivationInfo))
			{
				FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
				FGameplayCueParameters CueParams;
				// 히트 결과가 있다면 해당 위치를, 없다면 기본 사거리 끝 지점을 사용하도록 TA에서 데이터를 잘 만들어줌
				const FHitResult* HitResult = DataHandle.Get(0)->GetHitResult();
				if (HitResult)
				{
					CueParams.Location = HitResult->ImpactPoint;
					// Trace 시작점이나 방향 정보가 필요하다면 Normal이나 Origin 등을 활용 가능
					CueParams.Normal = HitResult->ImpactNormal;
				}

				// 시전자 정보를 넘겨서 시작 위치(총구 등)를 계산할 수 있게 함
				CueParams.Instigator = Avatar;

				// 아이템 데이터를 SourceObject로 넘겨서, GameplayCue에서 나이아가라 에셋을 꺼내 쓸 수 있게 함
				CueParams.SourceObject = CurrentItemData;

				ASC->ExecuteGameplayCue(FZFGameplayTags::GameplayCue_Weapon_Laser, CueParams);
			}

			TArray<TSubclassOf<UGameplayEffect>> EffectsToApply; // 적용할 효과 클래스담는 배열
			if (CurrentItemData)
			{
				EffectsToApply = CurrentItemData->AllowedEffectClasses;
			}
			// 플레이어 데이터가 없고 몬스터라면 몬스터 데이터의 GE 확인
			else if (AFZFMonster* Monster = Cast<AFZFMonster>(Avatar))
			{
				if (UFZFMonsterData* MData = Monster->GetMonsterData())
				{
					EffectsToApply = MData->AllowedEffectClasses;
				}
			}

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
							// 현재 장착 무기의 AttackTag를 이펙트 스펙에 동적으로 주입
							if (HeldItemComp)
							{
								FGameplayTag WeaponAttackTag = HeldItemComp->GetCurrentAttackTag();
								if (WeaponAttackTag.IsValid())
								{
									NewSpecHandle.Data.Get()->AppendDynamicAssetTags(FGameplayTagContainer(WeaponAttackTag));
									UE_LOG(LogTemp, Log, TEXT("[AttackHitCheck] 성공: 무기 동적 태그 주입 -> %s"), *WeaponAttackTag.ToString());
								}
							}

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

	//// 모든 로직이 완료되면 능력을 종료
	//EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UFZFGA_AttackHitCheck::OnTargetDataCancelled(const FGameplayAbilityTargetDataHandle& DataHandle)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}
