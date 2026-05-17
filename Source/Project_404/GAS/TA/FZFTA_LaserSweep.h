// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/TA/FZFTA_Base.h"
#include "FZFTA_LaserSweep.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_404_API AFZFTA_LaserSweep : public AFZFTA_Base
{
	GENERATED_BODY()
public:
	AFZFTA_LaserSweep();

	// 상위 Ability애서 Confirm을 눌렀을 때, 실행되는 함수
	virtual void ConfirmTargetingAndContinue() override;

	// 타겟팅을 시작
	virtual void StartTargeting(UGameplayAbility* Ability) override;

	void SetShowDebug(bool InShowDebug) { bShowDebug = InShowDebug; }

protected:
	// FGameplayAbilityTargetDataHandle을 리턴 (타겟데이터의 묶음)
	virtual FGameplayAbilityTargetDataHandle MakeTargetData() const;
};
