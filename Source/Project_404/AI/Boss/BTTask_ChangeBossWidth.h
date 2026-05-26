// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ChangeBossWidth.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_404_API UBTTask_ChangeBossWidth : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_ChangeBossWidth();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Mesh Width")
	bool bUseBlackboardTargetY = true;

	UPROPERTY(EditAnywhere, Category = "Mesh Width")
	float TargetMeshY = 0.f;

	UPROPERTY(EditAnywhere, Category = "Mesh Width")
	float InterpSpeed = 5.f;

	UPROPERTY(EditAnywhere, Category = "Mesh Width")
	float Tolerance = 3.f;
};
