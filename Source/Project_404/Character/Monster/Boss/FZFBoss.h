// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/FZFCharacterBase.h"
#include "Interface/FZFBossAIInterface.h"
#include "GameplayEffectTypes.h"
#include "Character/Monster/MonsterData/FZFBossData.h" // 이거 보스 전용으로 변경
#include "FZFBoss.generated.h"

class AFZFBossLevelManager;
class AFZFEnergyRelay;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBossWaitingEvent);

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

	// 동력원 파괴 델리게이트 초기화
	void BindEnergyRelayEvents();

public:
	// BossData 불러오기 함수
	FORCEINLINE virtual UFZFBossData* GetData() override
	{
		return BossData ? BossData : nullptr;
	};
	
	//Boss AttributeSet 게터 함수
	FORCEINLINE class UFZFMonsterSet* GetBossAttributeSet() const 
	{
		return BossAttributeSet; 
	}

	// BossMesh 가져오기.
	FORCEINLINE virtual USkeletalMeshComponent* GetBossMesh() const override
	{
		return GetMesh();
	};

	/* 인터페이스 */
protected:
	// BT 전달 함수
	FORCEINLINE virtual UBehaviorTree* GetBT() override
	{
		return BossData ? BossData->BehaviorTree : nullptr;
	};

	// 선택된 스킬 저장/불러오기 함수
	FORCEINLINE void SetCurrentSelectedSkill(const FBossSkillInfo& Skill) override
	{
		CurrentSelectedSkill = Skill;
	};
	FORCEINLINE const FBossSkillInfo* GetCurrentSelectedSkill() const override
	{
		return &CurrentSelectedSkill;
	};


	// Task에서 공격 처리 호출 함수
	virtual void SetAIAttackDelegate(const FBossAICharacterAttackFinished& InOnAttackFinished) override;
	virtual void AttackByAI() override;
	virtual void RequestMapPattern(const FBossSkillInfo& Skill) override;

	// 공격 종료/정리 함수.
	virtual void ResetBossAction() override;

	// 외부 동력원 델리게이트 전달 함수.
	UFUNCTION()
	void NotifyWaitingStarted() override;
	UFUNCTION()
	void NotifyWaitingEnded() override;

	// 페이즈 전환 함수.
	virtual void OnBossPhaseTransition(int32 NewPhase) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_BossPhase();

	/* 클래스 멤버 함수*/
public:
	// 공격 모션(몽타주 재생) 종료 시 호출되는 이벤트 함수.
	void NotifyAttackActionEnd();

	void StopMapPattern();

protected:
	// 동력원 파괴 델리게이트 호출 함수
	UFUNCTION()
	void HandleEnergyRelayDestroyed(AFZFEnergyRelay* Relay);

	// 보스 함정 델리게이트 호출 함수
	UFUNCTION()
	void HandleBossBombCreated(FVector BombLocation);

	// 죽음 처리 함수
	virtual void SetDead() override;

	/* 클래스 멤버 변수 */
public:
	// 외부 동력원 델리게이트 호출 관련
	UPROPERTY(BlueprintAssignable, Category = "Boss|Event")
	FBossWaitingEvent OnBossWaitingStarted;

	UPROPERTY(BlueprintAssignable, Category = "Boss|Event")
	FBossWaitingEvent OnBossWaitingEnded;

private:
	// 초기화 순서 체크 플래그
	bool bBeginPlayReady = false;
	bool bPossessedReady = false;
	bool bMonsterInitialized = false;

protected:
	UPROPERTY(ReplicatedUsing = OnRep_BossPhase)
	int32 BossPhase = 1;

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

	// 현재 선택된 스킬 정보
	UPROPERTY()
	FBossSkillInfo CurrentSelectedSkill;

	// 맵 패턴 공격 관련
	UPROPERTY(EditAnywhere, Category = "Boss|MapPattern")
	TObjectPtr<AFZFBossLevelManager> BossLevelManager;

	FTimerHandle MapPatternTimerHandle;

	// 고리 매시 저장
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Ring")
	TArray<TObjectPtr<USkeletalMeshComponent>> RingMeshes;

	// 동력원 이벤트 구독 관련
	UPROPERTY(EditAnywhere, Category = "Boss|Relay")
	TArray<TObjectPtr<AFZFEnergyRelay>> EnergyRelays;

	// 부서진 고리
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Ring")
	TArray<TSubclassOf<AActor>> BrokenRingActorClasses;

	// 죽은 후 대기할 시간 값(단위: 초).
	float DeadEventDelayTime = 7.0f;
};
