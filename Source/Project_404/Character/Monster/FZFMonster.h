// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/FZFCharacterBase.h"
#include "Interface/FZFMonsterAIInterface.h"
#include "GameplayEffectTypes.h"
#include "Character/Monster/MonsterData/FZFMonsterData.h"
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

	// MonsterData Getter
	FORCEINLINE UFZFMonsterData* GetMonsterData() const { return MonsterData; }

protected:
	/* 클래스 멤버 함수(초기화) */
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	
	// 몬스터 종류별 초기화
	virtual void InitializeMonster();

	// GAS 관련 초기화
	virtual void InitAbilitySystem() override;

	// AttributeSet 초기화
	virtual void InitAttributesFromData();

	

	/* 인터페이스 */
	virtual float GetAIPatrolRadius() override;
	virtual float GetAIDetectRange() override;
	virtual float GetAIAttackRange() override;
	virtual float GetAIAttackDetectRange() override;
	virtual float GetAITurnSpeed() override;

	// BT 전달 함수
	virtual UBehaviorTree* GetBT() override;

	 // Task에서 공격 처리 호출 함수
	virtual void SetAIAttackDelegate(const FAICharacterAttackFinished& InOnAttackFinished) override;
	virtual void AttackByAI() override;

	// Task에서 MoveSpeed Change 함수
	virtual void SetAIMoveSpeedMode(EFZFAIMoveSpeedMode MoveSpeedMode) override;
	
	/* 클래스 멤버 함수 */
public:
	// 공격 모션(몽타주 재생) 종료 시 호출되는 이벤트 함수.
	void NotifyAttackActionEnd();

	/* 클래스 멤버 변수 */
private:
	// 초기화 순서 체크 플래그
	bool bBeginPlayReady = false;
	bool bPossessedReady = false;
	bool bMonsterInitialized = false;

protected:
	// SetAIAttackDelegate 함수로부터 전달받은 델리게이트를 저장할 변수.
	FAICharacterAttackFinished OnAttackFinished;

	// 몬스터 AttributeSet 할당
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = MonsterSet)
	TObjectPtr<class UFZFMonsterSet> MonsterAttributeSet;

	// MonsterAttributeSet의 기본 능력치 초기화 Init_GE
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GAS)
	TSubclassOf<class UGameplayEffect> InitMonsterEffectClass;

	// 이동속도 전환 버프용 GE(Infinite Duration)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GAS)
	TSubclassOf<class UGameplayEffect> ChaseSpeedEffectClass;

	// 스프린트 버프 GE 핸들 (끝낼 때 제거용)
	FActiveGameplayEffectHandle ChaseSpeedEffectHandle;

	// 몬스터 데이터 에셋 저장 변수.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data)
	TObjectPtr<UFZFMonsterData> MonsterData;
};
