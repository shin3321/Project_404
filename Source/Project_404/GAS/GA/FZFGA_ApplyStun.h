// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "FZFGA_ApplyStun.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_404_API UFZFGA_ApplyStun : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UFZFGA_ApplyStun();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	// 태그가 사라졌을 때 호출될 콜백 함수
	UFUNCTION()
	void OnStunTagRemoved();
	
};
