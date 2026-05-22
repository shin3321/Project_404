// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "FZFMMC_AttackDamage.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_404_API UFZFMMC_AttackDamage : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
	
public:
	UFZFMMC_AttackDamage();

	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

private:
	// 공격자의 공격력을 가져올 변수
	FGameplayEffectAttributeCaptureDefinition AttackDef;
};
