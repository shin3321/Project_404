// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "FZFAIController.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_404_API AFZFAIController : public AAIController
{
	GENERATED_BODY()

public:
	AFZFAIController();

	// AI 실행 함수.
	void RunAI();

	// AI 중단 함수.
	void StopAI();

protected:
	// 컨트롤러가 폰에 빙의했을 때 호출되는 함수.
	// BT/BB 애셋 설정 및 실행 처리.
	virtual void OnPossess(APawn* InPawn) override;

	
private:
	// 블랙보드 애셋.
	UPROPERTY()
	TObjectPtr<class UBlackboardData> BBAsset;

	// 비헤이비어 트리 에셋.
	UPROPERTY()
	TObjectPtr<class UBehaviorTree> BTAsset;
};
