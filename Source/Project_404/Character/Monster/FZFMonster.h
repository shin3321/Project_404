// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/FZFCharacterBase.h"
#include "Interface/FZFMonsterAIInterface.h"
#include "FZFMonster.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_404_API AFZFMonster : public AFZFCharacterBase, public IFZFMonsterAIInterface
{
	GENERATED_BODY()

public:
	AFZFMonster();

protected:
	virtual void BeginPlay() override;

	virtual void InitAbilitySystem() override;

	

	// Inherited via IFZFMonsterAIInterface
	virtual float GetAIPatrolRadius() override;
	virtual float GetAIDetectRange() override;
	virtual float GetAIAttackRange() override;
	virtual float GetAITurnSpeed() override;

	virtual void AttackByAI() override; 
	virtual void SetAIAttackDelegate(const FAICharacterAttackFinished& InOnAttackFinished) override;

	// SetAIAttackDelegate 함수로부터 전달받은 델리게이트를 저장할 변수.
	FAICharacterAttackFinished OnAttackFinished;

	// 몬스터 AttributeSet 할당
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = MonsterSet)
	TObjectPtr<class UFZFMonsterSet> MonsterAttributeSet;
};
