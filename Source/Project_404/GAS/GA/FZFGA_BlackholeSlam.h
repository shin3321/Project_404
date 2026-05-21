// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "FZFGA_BlackholeSlam.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_404_API UFZFGA_BlackholeSlam : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UFZFGA_BlackholeSlam();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	// 몽타주 재생 종료
	UFUNCTION()
	void OnMontageCompleted();

	UFUNCTION()
	void OnMontageCancelled();

	UPROPERTY(EditDefaultsOnly, Category = "Blackhole")
	TObjectPtr<class UAnimMontage> BlackholeSlamMontage;

};
