// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Attributes/FZFMonsterSet.h"
#include "Net/UnrealNetwork.h"

UFZFMonsterSet::UFZFMonsterSet()
{

	// 이동 스피드 설정.
	InitMovementSpeed(300.0f);

	// 공격 반경 설정.
	InitAttackRadius(50.0f);

	// 공격 사거리 설정.
	InitAttackRange(80.0f);

	// 공격 속도 설정.
	InitAttackSpeed(1.5f);

	// 플레이어 감지 범위 설정.
	InitDetectRange(400.0f);

	// 회전 스피드 설정.
	InitTurnSpeed(2.0f);
}

void UFZFMonsterSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UFZFMonsterSet, DetectRange, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UFZFMonsterSet, TurnSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UFZFMonsterSet, PullStrength, COND_None, REPNOTIFY_Always);
}

void UFZFMonsterSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
}

void UFZFMonsterSet::OnRep_DetectRange(const FGameplayAttributeData& OldDetectRange)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFZFMonsterSet, DetectRange, OldDetectRange);
}

void UFZFMonsterSet::OnRep_TurnSpeed(const FGameplayAttributeData& OldTurnSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFZFMonsterSet, TurnSpeed, OldTurnSpeed);
}

void UFZFMonsterSet::OnRep_PullStrength(const FGameplayAttributeData& OldPullStrength)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFZFMonsterSet, PullStrength, OldPullStrength);
}
