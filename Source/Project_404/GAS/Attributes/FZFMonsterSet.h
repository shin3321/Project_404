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
	// 끌어당기는 힘(Pull Strength)
	ATTRIBUTE_ACCESSORS(UFZFMonsterSet, PullStrength);

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

protected:
	UFUNCTION()
	virtual void OnRep_DetectRange(const FGameplayAttributeData& OldDetectRange);

	UFUNCTION()
	virtual void OnRep_TurnSpeed(const FGameplayAttributeData& OldTurnSpeed);

	UFUNCTION()
	virtual void OnRep_PullStrength(const FGameplayAttributeData& OldPullStrength);

private:
	UPROPERTY(BlueprintReadOnly, Category = Stat, ReplicatedUsing = OnRep_DetectRange, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData DetectRange;

	UPROPERTY(BlueprintReadOnly, Category = Stat, ReplicatedUsing = OnRep_TurnSpeed, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData TurnSpeed;
	
	UPROPERTY(BlueprintReadOnly, Category = Stat, ReplicatedUsing = OnRep_PullStrength, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData PullStrength;
};
