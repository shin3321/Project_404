// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Interface/FZFMonsterAIInterface.h"
#include "BTTask_ChangeMoveSpeed.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_404_API UBTTask_ChangeMoveSpeed : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_ChangeMoveSpeed();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere, Category = "MoveType")
	EFZFAIMoveSpeedMode MoveSpeedMode = EFZFAIMoveSpeedMode::Patrol;
	
};
