// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "FZFMonsterAIInterface.generated.h"

class UBehaviorTree;

// 공격 종료 델리게이트 선언.
DECLARE_DELEGATE(FAICharacterAttackFinished);

// MoveSpeed Enum
UENUM(BlueprintType)
enum class EFZFAIMoveSpeedMode : uint8
{
	Patrol,
	Chase
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UFZFMonsterAIInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECT_404_API IFZFMonsterAIInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// 몬스터가 구현해야하는 함수.
	// 정찰 범위
	virtual float GetAIPatrolRadius() = 0;
	// 플레이어 감지 범위
	virtual float GetAIDetectRange() = 0;
	// 공격 사거리
	virtual float GetAIAttackRange() = 0;
	// 공격 플레이어 감지 범위
	virtual float GetAIAttackDetectRange() = 0;
	// 공격 때 회전 스피드
	virtual float GetAITurnSpeed() = 0;

	// AI 컨트롤러에 BT 전달 함수.
	virtual UBehaviorTree* GetBT() = 0;

	// Task에서 공격 명령을 전달할 때 사용하는 함수.
	virtual void AttackByAI() = 0;

	// 캐릭터에서 델리게이트를 넘길 때 사용할 함수.
	virtual void SetAIAttackDelegate(const FAICharacterAttackFinished& InOnAttackFinished) = 0;

	// MoveSpeed 설정 함수.
	virtual void SetAIMoveSpeedMode(EFZFAIMoveSpeedMode MoveSpeedMode) = 0;
};
