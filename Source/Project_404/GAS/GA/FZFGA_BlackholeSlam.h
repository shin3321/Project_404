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

	// Pulling 종료 후 점프 시작
	UFUNCTION()
	void OnPullFinished();

	// 착지 시, 데미지 이벤트 발생 
	UFUNCTION()
	void OnSlamHitEventReceived(FGameplayEventData Payload);

	// 몽타주 재생 종료
	UFUNCTION()
	void OnMontageCompleted();

	// 스킬 설정 변수들
	UPROPERTY(EditDefaultsOnly, Category = "Blackhole")
	float PullDuration = 2.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Blackhole")
	float SlamRadius = 500.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Blackhole")
	TSubclassOf<class UGameplayEffect> SlamDamageEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Blackhole")
	class UAnimMontage* BlackholeSlamMontage;

private:
	FTimerHandle PullTimerHandle;
};
