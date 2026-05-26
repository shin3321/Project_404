// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "FZFGA_AttackHitCheck_Channeling.generated.h"

UCLASS()
class PROJECT_404_API UFZFGA_AttackHitCheck_Channeling : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UFZFGA_AttackHitCheck_Channeling();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UFUNCTION()
	void OnTargetDataReceived(const FGameplayAbilityTargetDataHandle& DataHandle);

	// 주기적으로 레이저 판정을 수행할 타이머 함수
	void PerformLaserTick();

	UPROPERTY(EditAnywhere, Category = "GAS")
	TSubclassOf<class AFZFTA_Base> TargetActorClass;

	UPROPERTY(EditAnywhere, Category = "GAS")
	float LaserDuration; // 레이저 지속 시간
	
	UPROPERTY(EditAnywhere, Category = "GAS")
	float TraceInterval; // 판정 주기

private:
	UPROPERTY()
	TObjectPtr<class AFZFTA_Base> SpawnedTargetActor;

	FTimerHandle LaserTimerHandle;
	FTimerHandle LaserDurationTimerHandle;

	// 일회성 데미지를 위해 이미 피격된 액터들을 기억하는 저장소
	TSet<TWeakObjectPtr<AActor>> AlreadyHitActors;
};
