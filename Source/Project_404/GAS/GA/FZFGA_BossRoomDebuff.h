// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "FZFGA_BossRoomDebuff.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_404_API UFZFGA_BossRoomDebuff : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UFZFGA_BossRoomDebuff();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	// 주기적으로 체력을 감소 시킬 GameplayEffect 클래스
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Debuff")
	TSubclassOf<class UGameplayEffect> DOTEffectClass;

	// 생성된 효과를 나중에 해제하기 위해 저장해둘 핸들
	FActiveGameplayEffectHandle ActiveDOTEffectHandle;
};
