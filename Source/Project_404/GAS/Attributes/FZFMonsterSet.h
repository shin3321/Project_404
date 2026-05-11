// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Attributes/FZFAttributeSet.h"
#include "FZFMonsterSet.generated.h"

UCLASS()
class PROJECT_404_API UFZFMonsterSet : public UFZFAttributeSet
{
	GENERATED_BODY()

public:
	UFZFMonsterSet();
	 
	// 탐지 거리 (Detection Range)
	ATTRIBUTE_ACCESSORS(UFZFMonsterSet, DetectRange);
	// 회전 속도 (Turn Speed)
	ATTRIBUTE_ACCESSORS(UFZFMonsterSet, TurnSpeed);

private:
	UPROPERTY(BlueprintReadOnly, Category = Stat, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData DetectRange;

	UPROPERTY(BlueprintReadOnly, Category = Stat, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData TurnSpeed;
};
