// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ChangeRotate.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_404_API UBTTask_ChangeRotate : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_ChangeRotate();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Boss Rotate")
	bool bUseBlackboardValue = true;

	// 현재 Actor Yaw 기준으로 얼마나 돌릴지
	// 180 = 뒤돌기, 90 = 오른쪽, -90 = 왼쪽
	UPROPERTY(EditAnywhere, Category = "Boss Rotate")
	float TargetYawOffset = 0.f;

	// Mesh를 위/아래로 얼마나 숙일지
	// 아래 보기: 보통 -30
	UPROPERTY(EditAnywhere, Category = "Boss Rotate")
	float TargetMeshPitch = 0.f;

	UPROPERTY(EditAnywhere, Category = "Boss Rotate")
	float ActorYawInterpSpeed = 5.f;

	UPROPERTY(EditAnywhere, Category = "Boss Rotate")
	float MeshPitchInterpSpeed = 5.f;

	UPROPERTY(EditAnywhere, Category = "Boss Rotate")
	float YawTolerance = 2.f;

	UPROPERTY(EditAnywhere, Category = "Boss Rotate")
	float PitchTolerance = 2.f;

private:
	float CachedTargetYaw = 0.f;
	float CachedTargetMeshPitch = 0.f;
};