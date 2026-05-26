// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/MMC/FZFMMC_AttackDamage.h"
#include "GAS/Attributes/FZFAttributeSet.h"
#include "GAS/Attributes/FZFMonsterSet.h"
#include "AbilitySystemComponent.h"
#include "Character/Monster/Boss/FZFBoss.h"
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

    // 공격자의 본연의 캐릭터 정보 가져오기
    AActor* SourceActor = Spec.GetContext().GetInstigator();
    float BaseAttack = 0.0f;

    // GAS 방식으로 안전하게 공격력 추출 시도
    bool bCaptureSuccess = false;

    // 만약 일반 MonsterSet을 쓰는 대상이라면 몬스터 주머니 우선 캡처
    if (SourceActor && !SourceActor->IsA<APawn>())
    {
		bCaptureSuccess = GetCapturedAttributeMagnitude(AttackDef, Spec, EvaluationParameters, BaseAttack);
    }

    // 위에서 실패했거나 플레이어라면 기본 주머니에서 캡처
    if (!bCaptureSuccess || BaseAttack <= 0.0f)
    {
		bCaptureSuccess = GetCapturedAttributeMagnitude(AttackDef, Spec, EvaluationParameters, BaseAttack);
    }

    // GAS 시스템이 엇박자가 나서 캡처를 둘 다 실패했다?
    // 공격자 액터를 직접 보스클래스로 다운캐스팅해서 C++에 저장된 스텟을 강제로 뜯어옴
    if (BaseAttack <= 0.0f && SourceActor)
    {
        if (AFZFBoss* Boss = Cast<AFZFBoss>(SourceActor))
        {
            // 보스가 가진 BossAttributeSet 주머니에서 직접 실시간 공격력을 Get해옵니다.
            if (Boss->GetBossAttributeSet())
            {
                BaseAttack = Boss->GetBossAttributeSet()->GetAttack();
                UE_LOG(LogTemp, Warning, TEXT("[MMC_Fix] GAS 캡처 실패로 보스 AttributeSet에서 직접 깡으로 공격력 추출 성공! -> %.2f"), BaseAttack);
            }
        }
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

    UE_LOG(LogTemp, Log, TEXT("[MMC_Fix] 최종 계산되어 투입되는 데미지 수치: %.2f"), BaseAttack);

    // 쿨타임이 아니면 기본 공격력 100% 반환
    return BaseAttack;
}
