// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Attributes/FZFMonsterSet.h"

UFZFMonsterSet::UFZFMonsterSet()
{
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

	// 이동 스피드 설정.
	InitMovementSpeed(300.0f);
}
