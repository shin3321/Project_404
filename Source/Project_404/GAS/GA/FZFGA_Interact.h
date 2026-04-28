// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "FZFGA_Interact.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_404_API UFZFGA_Interact : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UFZFGA_Interact();

	// 어빌리티가 활성화될 때 실행되는 메인 함수
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	// 트레이스 거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interact")
	float TraceDistance = 500.0f;

	// 실제 아이템 획득 로직을 처리할 내부 함수
	void PerformTraceAndPickup();
};
