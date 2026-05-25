// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "FZFBossAIInterface.generated.h"

class UBehaviorTree;
class UFZFBossData;
struct FBossSkillInfo;
class USkeletalMeshComponent;

// 공격 종료 델리게이트 선언.
DECLARE_DELEGATE(FBossAICharacterAttackFinished);

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UFZFBossAIInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECT_404_API IFZFBossAIInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	// AI 컨트롤러에 BT 전달 함수.
	virtual UBehaviorTree* GetBT() = 0;

	// BossData전달 함수.
	virtual UFZFBossData* GetData() = 0;

	// BossMesh 가져오기
	virtual USkeletalMeshComponent* GetBossMesh() const = 0;

	// 선택된 스킬 저장 함수.
	virtual void SetCurrentSelectedSkill(const FBossSkillInfo& Skill) = 0;

	// 선택된 스킬 전달 함수.
	virtual const FBossSkillInfo* GetCurrentSelectedSkill() const = 0;

	// Task에서 공격 명령을 전달할 때 사용하는 함수.
	virtual void AttackByAI() = 0;

	// Task에서 맵 패턴 공격 명령을 전달할 때 사용하는 함수.
	virtual void RequestMapPattern(const FBossSkillInfo& Skill) = 0;

	// 캐릭터에서 델리게이트를 넘길 때 사용할 함수.
	virtual void SetAIAttackDelegate(const FBossAICharacterAttackFinished& InOnAttackFinished) = 0;

	// 공격 종료/정리 함수.
	virtual void ResetBossAction() = 0;

	// 외부 동력원 호출 델리게이트 전달.
	virtual void NotifyWaitingStarted() = 0;
	virtual void NotifyWaitingEnded() = 0;

	// 페이즈 전환 호출 함수.
	virtual void OnBossPhaseTransition(int32 NewPhase) = 0;
};
