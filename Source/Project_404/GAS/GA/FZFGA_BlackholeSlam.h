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

	// 0.1초마다 주변 플레이어를 당기는 핵심 로직
	void PullTargets();

	// Pulling 종료 후 점프 시작
	UFUNCTION()
	void OnPullFinished();

	// 몽타주 재생 종료
	UFUNCTION()
	void OnMontageCompleted();

	// 스킬 설정 변수들
	UPROPERTY(EditDefaultsOnly, Category = "Blackhole")
	float PullDuration = 1.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Blackhole")
	float PullStrength = 400.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Blackhole")
	float SlamRadius = 500.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Blackhole")
	class UAnimMontage* BlackholeSlamMontage;

private:
	// 타이머를 역할별로 분리
	FTimerHandle PullTimerHandle; // 끌어당기는 Timer
	FTimerHandle PhaseTimerHandle; // Phase 전환 Timer
};
