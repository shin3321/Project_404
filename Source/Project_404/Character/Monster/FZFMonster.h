// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/FZFCharacterBase.h"
#include "Interface/FZFMonsterAIInterface.h"
#include "GameplayEffectTypes.h"
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
	virtual float GetAIAttackDetectRange() override;
	virtual float GetAITurnSpeed() override;

	 // Task에서 공격 처리 호출 함수
	virtual void SetAIAttackDelegate(const FAICharacterAttackFinished& InOnAttackFinished) override;
	virtual void AttackByAI() override;

	// Task에서 MoveSpeed Change 함수
	virtual void SetAIMoveSpeedMode(EFZFAIMoveSpeedMode MoveSpeedMode) override;
	

public:
	// 공격 모션(몽타주 재생) 종료 시 호출되는 이벤트 함수.
	void NotifyAttackActionEnd();


protected:

	// SetAIAttackDelegate 함수로부터 전달받은 델리게이트를 저장할 변수.
	FAICharacterAttackFinished OnAttackFinished;

	// 몬스터 AttributeSet 할당
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = MonsterSet)
	TObjectPtr<class UFZFMonsterSet> MonsterAttributeSet;

	// 공격 몽타주 할당
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attack)
	TObjectPtr<class UAnimMontage> AttackMontage;

	// 이동속도 전환 버프용 GE(Infinite Duration)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GAS)
	TSubclassOf<class UGameplayEffect> ChaseSpeedEffectClass;

	// 스프린트 버프 GE 핸들 (끝낼 때 제거용)
	FActiveGameplayEffectHandle ChaseSpeedEffectHandle;
};
