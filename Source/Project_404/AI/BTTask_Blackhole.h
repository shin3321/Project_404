// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_Blackhole.generated.h"

UCLASS()
class PROJECT_404_API UBTTask_Blackhole : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_Blackhole();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	// 매 프레임 당기기 연산을 수행할 TickTask
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	// AttributeSet에서 가져올 수치들을 저장할 변수들
	float PullDuration; // 끌어당기는 시간
	float PullStrength; // 끌어당기는 힘
	float PullRange; // 끌어당기는 범위

	float ElapsedTime; // 경과 시간 체크용
};
