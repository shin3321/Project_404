// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA/FZFGA_AttackHitCheck_Channeling.h"
#include "GAS/TA/FZFTA_Base.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Character/Monster/FZFMonster.h"
#include "Character/Monster/Boss/FZFBoss.h"
#include "AbilitySystemComponent.h"
#include "GameplayTag/FZFGameplayTags.h"

UFZFGA_AttackHitCheck_Channeling::UFZFGA_AttackHitCheck_Channeling()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UFZFGA_AttackHitCheck_Channeling::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	// 타이머 중복 실행 방지 선행 안전조치 복구
	if (GetWorld()->GetTimerManager().IsTimerActive(LaserTimerHandle))
	{
		return;
	}

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
	SpawnedTargetActor = GetWorld()->SpawnActor<AFZFTA_Base>(TargetActorClass);
	if (SpawnedTargetActor)
	{
		SpawnedTargetActor->SourceActor = Avatar;
		SpawnedTargetActor->bShowDebug = true;

		if (AFZFBoss* Boss = Cast<AFZFBoss>(Avatar))
		{
			if (UFZFBossData* BData = Boss->GetData())
			{
				SpawnedTargetActor->bUseSocket = true;
				SpawnedTargetActor->StartSocketName = BData->AttackSocket;
			}
		}
		else if (AFZFMonster* Monster = Cast<AFZFMonster>(Avatar))
		{
			if (UFZFMonsterData* MData = Monster->GetMonsterData())
			{
				SpawnedTargetActor->bUseSocket = true;
				SpawnedTargetActor->StartSocketName = MData->AttackSocket;
			}
		}
	}

	// WaitTargetData 태스크 구동
	UAbilityTask_WaitTargetData* WaitTargetDataTask = UAbilityTask_WaitTargetData::WaitTargetDataUsingActor(
		this,
		NAME_None,
		EGameplayTargetingConfirmation::CustomMulti, // 루프제어를 위해서 Custom으로 설정
		SpawnedTargetActor
	);

	if (WaitTargetDataTask)
	{
		WaitTargetDataTask->ValidData.AddDynamic(this, &UFZFGA_AttackHitCheck_Channeling::OnTargetDataReceived);
		WaitTargetDataTask->ReadyForActivation();
	}
	// 0.05초 간격 반복 추적 타이머
	GetWorld()->GetTimerManager().SetTimer(LaserTimerHandle, this, &UFZFGA_AttackHitCheck_Channeling::PerformLaserTick, TraceInterval, true);

	// 지속시간 뒤에 어빌리티가 자동으로 꺼지도록 타이머 설정
	GetWorld()->GetTimerManager().SetTimer(LaserDurationTimerHandle, FTimerDelegate::CreateUObject(this, &UFZFGA_AttackHitCheck_Channeling::EndAbility, Handle, ActorInfo, ActivationInfo, true, false), LaserDuration, false);

}

void UFZFGA_AttackHitCheck_Channeling::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// 타이머들을 안전하게 정리
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(LaserTimerHandle);
		GetWorld()->GetTimerManager().ClearTimer(LaserDurationTimerHandle);
	}

	// 생성했던 타겟 액터 제거
	if (SpawnedTargetActor)
	{
		SpawnedTargetActor->Destroy();
	}

	// 루프 이펙트를 꺼주기 위해 큐 제거 또는 스톱 이벤트 처리 (프로젝트 구조에 맞게)

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UFZFGA_AttackHitCheck_Channeling::PerformLaserTick()
{
	if (SpawnedTargetActor)
	{
		// 노티파이 틱 신호가 올때마다 즉시 타겟 액터의 실시간 판정을 갱신하라고 명령
		SpawnedTargetActor->ConfirmTargeting();
	}
}

void UFZFGA_AttackHitCheck_Channeling::OnTargetDataReceived(const FGameplayAbilityTargetDataHandle& DataHandle)
{
	UE_LOG(LogTemp, Warning, TEXT("[LaserDebug] ========================================================"));
	UE_LOG(LogTemp, Warning, TEXT("[LaserDebug] OnTargetDataReceived 진입! 들어온 타겟 데이터 개수: %d"), DataHandle.Num());

	AActor* Avatar = GetAvatarActorFromActorInfo();
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!Avatar || !ASC)
	{
		UE_LOG(LogTemp, Error, TEXT("[LaserDebug] 크리티컬 실패: Avatar(%p) 또는 ASC(%p)가 널입니다."), Avatar, ASC);
		return;
	}

	// GE목록 추출 로직 가져오기
	TArray<TSubclassOf<UGameplayEffect>> EffectsToApply;
	if (AFZFBoss* Boss = Cast<AFZFBoss>(Avatar))
	{
		if (auto* BData = Boss->GetData())
		{
			EffectsToApply = BData->AllowedEffectClasses;
			UE_LOG(LogTemp, Error, TEXT("[LaserDebug] 보스 데이터(BData) 로드 성공! 에셋 내 GE 개수: %d"), EffectsToApply.Num());
		}
	}
	else if (AFZFMonster* Monster = Cast<AFZFMonster>(Avatar))
	{
		if (UFZFMonsterData* MData = Monster->GetMonsterData())
		{
			EffectsToApply = MData->AllowedEffectClasses;
			UE_LOG(LogTemp, Log, TEXT("[LaserDebug] 일반 몬스터 데이터 로드 성공. GE 개수: %d"), EffectsToApply.Num());
		}
	}

	for (int32 i = 0; i < DataHandle.Num(); ++i)
	{

		UE_LOG(LogTemp, Log, TEXT("[LaserDebug] [%d]번째 타겟 데이터 검사 시작"), i);

		if (!DataHandle.IsValid(i))
		{
			UE_LOG(LogTemp, Warning, TEXT("[LaserDebug] -> [%d]번 실패: DataHandle이 유효하지(IsValid) 않습니다."), i);
			continue;
		}

		const FHitResult* HitResult = DataHandle.Get(i)->GetHitResult();
		if (!HitResult)
		{
			UE_LOG(LogTemp, Warning, TEXT("[LaserDebug] -> [%d]번 실패: HitResult 포인터가 널(Null)입니다. 허공 처리용 더미 데이터일 수 있습니다."), i);
			continue;
		}

		AActor* HitActor = HitResult->GetActor();
		if (!HitActor)
		{
			UE_LOG(LogTemp, Warning, TEXT("[LaserDebug] -> [%d]번 걸림: 부딪힌 오브젝트가 배경(벽/바닥)이라 Actor가 없습니다. 데미지 스킵."), i);
			continue;
		}

		// 효과 적용
		for(const TSubclassOf<UGameplayEffect>& EffectClass : EffectsToApply)
		{
			if (!EffectClass)
			{
				UE_LOG(LogTemp, Error, TEXT("[LaserDebug] -> 효과 클래스가 널입니다."));
				continue;
			}

			FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
			EffectContext.AddInstigator(Avatar, Avatar);

			FGameplayEffectSpecHandle NewSpecHandle = ASC->MakeOutgoingSpec(EffectClass, GetAbilityLevel(), EffectContext);
			if (NewSpecHandle.IsValid())
			{
				UE_LOG(LogTemp, Log, TEXT("[LaserDebug] -> -> GE Spec 생성 성공. 타겟에게 적용을 명령합니다."));
				// 해당 타겟(i번째)에게만 정확하게 데미지 전송
				ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, NewSpecHandle, DataHandle);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("[LaserDebug] -> -> GE Spec 생성 실패!"));
			}
		
		}
	}

}