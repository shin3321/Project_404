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

	// 공격 처리 함수.
	// 공격을 처음 시작할 때 실행.
	void ProcessAttack();

	// 공격이 시작될 때 실행할 함수.
	void AttackActionBegin();

	// 몽타주 재생 종료 시 호출할 함수 (델리게이트와 연동).
	void AttackActionEnd(UAnimMontage* TargetMontage, bool bInterrupted);

	// 공격이 끝나는 지점을 알 수 있도록 함수 선언.
	void NotifyComboActionEnd();


protected:

	// SetAIAttackDelegate 함수로부터 전달받은 델리게이트를 저장할 변수.
	FAICharacterAttackFinished OnAttackFinished;

	// 몬스터 AttributeSet 할당
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = MonsterSet)
	TObjectPtr<class UFZFMonsterSet> MonsterAttributeSet;

	// 공격 몽타주 할당
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attack)
	TObjectPtr<class UAnimMontage> AttackMontage;
};
