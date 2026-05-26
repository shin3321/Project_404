// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/MMC/FZFMMC_AttackDamage.h"
#include "GAS/Attributes/FZFAttributeSet.h"
#include "GAS/Attributes/FZFMonsterSet.h"
#include "AbilitySystemComponent.h"
#include "GameplayTag/FZFGameplayTags.h"

UFZFMMC_AttackDamage::UFZFMMC_AttackDamage()
{
    AttackDef.AttributeToCapture = UFZFAttributeSet::GetAttackAttribute(); // 공격력Attribute 캡처
    AttackDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source; // 스킬을 쓴사람
    AttackDef.bSnapshot = false; // 타격 시점의 실시간 공격력 반영
    
    // 저장할 주요 속성 -> AttackDef 추가
    RelevantAttributesToCapture.Add(AttackDef);

    MonsterAttackDef.AttributeToCapture = UFZFMonsterSet::GetAttackAttribute();
    MonsterAttackDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;
    MonsterAttackDef.bSnapshot = false;
    RelevantAttributesToCapture.Add(MonsterAttackDef);
}

float UFZFMMC_AttackDamage::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
    // 캡처한 공격자의 기본 공격력 수치 가져오기
    const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
    const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

    // 스탯 평가용 파라미터 상자
    FAggregatorEvaluateParameters EvaluationParameters;
    EvaluationParameters.SourceTags = SourceTags;
    EvaluationParameters.TargetTags = TargetTags;

    float BaseAttack = 0.0f;
    GetCapturedAttributeMagnitude(AttackDef, Spec, EvaluationParameters, BaseAttack);
    
    if (BaseAttack <= 0.0f)
    {
        GetCapturedAttributeMagnitude(MonsterAttackDef, Spec, EvaluationParameters, BaseAttack);
    }
    // 공격자의 ASC 가져오기
    UAbilitySystemComponent* SourceASC = Spec.GetContext().GetInstigatorAbilitySystemComponent();
    if (SourceASC)
    {
        // 공격자가 쿨타임 태그를 가지고 있는지 확인!
        FGameplayTag CooldownTag = FZFGameplayTags::Cooldown_Attack_Sword;

        if (SourceASC->HasMatchingGameplayTag(CooldownTag))
        {
            // 쿨타임 중이면 기본 공격력의 10%만 데미지로 반환
            return BaseAttack * 0.1f;
        }
    }

    // 쿨타임이 아니면 기본 공격력 100% 반환
    return BaseAttack;
}
