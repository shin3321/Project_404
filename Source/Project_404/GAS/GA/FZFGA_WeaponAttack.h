// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/GA/FZFGA_Attack.h"
#include "FZFGA_WeaponAttack.generated.h"

UCLASS()
class PROJECT_404_API UFZFGA_WeaponAttack : public UFZFGA_Attack
{
	GENERATED_BODY()
public:
    UFZFGA_WeaponAttack();
protected:
    // 공격 시작 시, 나에게 쿨타임 태그가 없는지 검사만 하고 기억해둠
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

    // 어빌리티가 종료될 때 로직을 끼워넣기 위해 EndAbility 오버라이드
    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

    // 무기 쿨다운 관련 변수들
    UPROPERTY(EditDefaultsOnly, Category = "GAS|SoftCooldown")
    FGameplayTag SoftCooldownTag;

    UPROPERTY(EditDefaultsOnly, Category = "GAS|SoftCooldown")
    TSubclassOf<class UGameplayEffect> SoftCooldownGEClass;

    bool bIsStrongAttack = false;
};
