// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/FZFCharacterBase.h"
#include "Interface/FZFBossAIInterface.h"
#include "GameplayEffectTypes.h"
#include "Character/Monster/MonsterData/FZFBossData.h" // 이거 보스 전용으로 변경
#include "FZFBoss.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_404_API AFZFBoss : public AFZFCharacterBase, public IFZFBossAIInterface
{
	GENERATED_BODY()
	
public:
	AFZFBoss();

protected:
	// 디버깅용 임시 테스트
	virtual void Tick(float DeltaTime) override;

	/* 클래스 멤버 함수(초기화) */
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;

	// MonsterData Asset 서버/클라 공통 초기화 (메시/애님/외형)
	virtual void InitializeBossVisual();

	void ApplyRingData(USkeletalMeshComponent* RingComp, int32 Index);

	// MonsterData Asset 서버만 초기화 (GAS/BT/Ability/AI)
	virtual void InitializeBossServer();

	// GAS 관련 초기화
	virtual void InitAbilitySystem() override;

	// AttributeSet 초기화
	virtual void InitAttributesFromData();

	/* 인터페이스 */
protected:
	// BT 전달 함수
	virtual UBehaviorTree* GetBT() override;

	// Task에서 공격 처리 호출 함수
	virtual void SetAIAttackDelegate(const FBossAICharacterAttackFinished& InOnAttackFinished) override;
	virtual void AttackByAI() override;

	/* 클래스 멤버 변수 */
private:
	// 초기화 순서 체크 플래그
	bool bBeginPlayReady = false;
	bool bPossessedReady = false;
	bool bMonsterInitialized = false;

protected:
	// SetAIAttackDelegate 함수로부터 전달받은 델리게이트를 저장할 변수.
	FBossAICharacterAttackFinished OnAttackFinished;

	// 몬스터 AttributeSet 할당
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = BossSet)
	TObjectPtr<class UFZFMonsterSet> BossAttributeSet; // 추후 보스 전용으로 변경

	// BossAttributeSet의 기본 능력치 초기화 Init_GE
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GAS)
	TSubclassOf<class UGameplayEffect> InitBossEffectClass;

	// 이동속도 전환 버프용 GE(Infinite Duration)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GAS)
	TSubclassOf<class UGameplayEffect> ChaseSpeedEffectClass;

	// 스프린트 버프 GE 핸들 (끝낼 때 제거용)
	FActiveGameplayEffectHandle ChaseSpeedEffectHandle;

	// 몬스터 데이터 에셋 저장 변수.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data)
	TObjectPtr<UFZFBossData> BossData;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Ring")
	TArray<TObjectPtr<USkeletalMeshComponent>> RingMeshes;

};
