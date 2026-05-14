// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA/FZFGA_Run.h"
#include "Character/Player/FZFCharacterPlayer.h"
#include "GAS/Attributes/FZFPlayerSet.h"
#include "GAS/FZFAbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitAttributeChangeThreshold.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "GameplayTag/FZFGameplayTags.h"

UFZFGA_Run::UFZFGA_Run()
{
    // 인스턴싱 정책: 어빌리티가 실행될 때마다 인스턴스를 생성 (데이터 관리가 편함)
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

    // 실행 정책 (반응성 강화)
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UFZFGA_Run::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    UFZFAbilitySystemComponent* ASC = Cast<UFZFAbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get());

    if (ASC && SprintBuffEffectClass)
    {
        // SprintBuffEffect 적용 및 핸들 저장
        FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
        FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(SprintBuffEffectClass, GetAbilityLevel(), EffectContext);
      
        if (SpecHandle.IsValid())
        {
            //ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
            // 위와 같이 쓰지말고 BP_버전으로 쓰면 SpecHandle 그대로 넘기면 됨
            // 방식 : Infinite 로 MovementSpeed Attribute를 증가시켜야함
            SprintBuffEffectHandle = ASC->BP_ApplyGameplayEffectSpecToSelf(SpecHandle);
        }
    
        // 스테미나 0 이하 감시 테스크
        UAbilityTask_WaitAttributeChangeThreshold* WaitTask =
            UAbilityTask_WaitAttributeChangeThreshold::WaitForAttributeChangeThreshold(
                this,
                UFZFPlayerSet::GetStaminaAttribute(),
                EWaitAttributeChangeComparison::LessThanOrEqualTo,
                0.0f,
                true
            );

        if (WaitTask)
        {
            WaitTask->OnChange.AddDynamic(this, &UFZFGA_Run::OnStaminaThresholdChanged);
            WaitTask->ReadyForActivation();
        }

        // 주기적 소모 루프 시작
        StartCostTickLoop();
    }
}

void UFZFGA_Run::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    UFZFAbilitySystemComponent* ASC = Cast<UFZFAbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get());

    if (SprintBuffEffectHandle.IsValid() && ASC)
    {
        // 적용했던 버프를 제거합니다. 
        ASC->RemoveActiveGameplayEffect(SprintBuffEffectHandle);
        SprintBuffEffectHandle.Invalidate();
    }

    // 2초간 회복 지연 이펙트 적용 -> 사실상 태그 + 지속시간 적용
    if (NoRegenEffectClass && ASC)
    {
        FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
        Context.AddSourceObject(ActorInfo->AvatarActor.Get());

        FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(NoRegenEffectClass, 1.0f, Context);
        if (SpecHandle.IsValid())
        {
            // 2초 뒤 자동으로 사라지는 GE 적용
            ASC->BP_ApplyGameplayEffectSpecToSelf(SpecHandle);
        }
    }

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UFZFGA_Run::OnStaminaEmpty(const FGameplayAttribute& Attribute, float NewValue, float OldValue)
{
    const FGameplayAbilityActorInfo* Info = GetCurrentActorInfo();
    EndAbility(CurrentSpecHandle, Info, CurrentActivationInfo, true, false);
}

void UFZFGA_Run::OnStaminaThresholdChanged(bool bMatchesComparison, float CurrentValue)
{
    // 스태미나가 0 이하인지 확인
    if (bMatchesComparison)
    {
        // 달리기 중단
        EndAbility(CurrentSpecHandle, GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
    }

    // TODO : 스테미나바 화면 테두리 붉게 변해도 좋을듯?
}

void UFZFGA_Run::OnCostTick()
{
    // 인자 없이 현재 이 능력을 실행 중인 주체의 ASC를 가지고 오고 싶을 때, GetAbilitySystemComponentFromActorInfo();
    UFZFAbilitySystemComponent* ASC = Cast<UFZFAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());

    // ASC가 없고 CostGameplayEffectClass가 없으면 return
    if (!ASC || !CostGameplayEffectClass)
    {
        return;
    }

    // 현재 달리는 상태가 아니면 return
    if (!ASC->HasMatchingGameplayTag(FZFGameplayTags::State_Movement_Run))
    {
        return;
    }

    if(!CommitAbilityCost(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo))
    {
        // 스테미나가 부족하면 알아서 종료
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
        return;
    }

    // 소모에 성공하면 다음 틱 예약
    StartCostTickLoop();
}

void UFZFGA_Run::StartCostTickLoop()
{
    if (!IsActive())
    {
        return;
    }

    UAbilityTask_WaitDelay* DelayTask = UAbilityTask_WaitDelay::WaitDelay(this, CostTickInterval);
    if (DelayTask)
    {
        DelayTask->OnFinish.AddDynamic(this, &UFZFGA_Run::OnCostTick);
        DelayTask->ReadyForActivation();
    }
}
