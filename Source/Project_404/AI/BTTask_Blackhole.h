// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "GameplayEffectTypes.h"
#include "BTTask_Blackhole.generated.h"

UCLASS()
class PROJECT_404_API UBTTask_Blackhole : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_Blackhole();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	// 비정상 강제 종료 시에도 청소하기 위해 AbortTask 오버라이드
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	// 디버프 일괄 해제용 공통 함수 선언
	void CleanupAllBlackholeEffects();

	// 매 프레임 당기기 연산을 수행할 TickTask
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

private:
	// AttributeSet에서 가져올 수치들을 저장할 변수들
	float PullDuration; // 끌어당기는 시간
	float PullStrength; // 끌어당기는 힘
	float PullRange; // 끌어당기는 범위

	float ElapsedTime; // 경과 시간 체크용
	
	// 에디터에서 만든 GE_Blackhole_Debuff를 지정할 변수
	UPROPERTY(EditAnywhere, Category = GAS)
	TSubclassOf<class UGameplayEffect> BlackholeGEClass;
	// 멀티 플레이어 대응 : 어떤 플레이어에게 어떤 이펙트 핸들이 들어갔는지 기억하는 명단 Map
	// Key: 플레이어 캐릭터, Value: 부여된 글로벌 이펙트 핸들 고유 번호
	TMap<TWeakObjectPtr<class ACharacter>, FActiveGameplayEffectHandle> ActiveGEHandles;

};
