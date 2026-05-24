// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ChangeBossMeshHeight.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_404_API UBTTask_ChangeBossMeshHeight : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_ChangeBossMeshHeight();
	
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp,uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Mesh Height")
	bool bUseBlackboardTargetZ = true;

	UPROPERTY(EditAnywhere, Category="Mesh Height")
	float TargetMeshZ = 0.f;
	
	UPROPERTY(EditAnywhere, Category="Mesh Height")
	float InterpSpeed = 5.f;
	
	UPROPERTY(EditAnywhere, Category="Mesh Height")
	float Tolerance = 3.f;
};
