// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA/FZFGA_Run.h"
#include "Character/Player/FZFCharacterPlayer.h"
#include "GAS/Attributes/FZFPlayerSet.h"
#include "GAS/FZFAbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

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

    AFZFCharacterBase* Character = Cast<AFZFCharacterBase>(ActorInfo->AvatarActor.Get());
    UFZFAbilitySystemComponent* ASC = Cast<UFZFAbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get());

    if (Character && ASC)
    {
        // AttributeSet에서 현재 속도를 가져옴
        float TargetRunSpeed = ASC->GetNumericAttribute(UFZFAttributeSet::GetMovementSpeedAttribute()) * 3.0f;

        // 캐릭터 무브먼트에 적용
        Character->GetCharacterMovement()->MaxWalkSpeed = TargetRunSpeed;
    }
}

void UFZFGA_Run::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    AFZFCharacterBase* Character = Cast<AFZFCharacterBase>(ActorInfo->AvatarActor.Get());
    UFZFAbilitySystemComponent* ASC = Cast<UFZFAbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get());

    if (Character && ASC)
    {
        // 다시 기본 이동 속도(WalkSpeed) 어트리뷰트 값을 가져와서 원복
        float DefaultWalkSpeed = ASC->GetNumericAttribute(UFZFAttributeSet::GetMovementSpeedAttribute());
        Character->GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;
    }

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
