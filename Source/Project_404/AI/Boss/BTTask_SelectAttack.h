// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_SelectAttack.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_404_API UBTTask_SelectAttack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_SelectAttack();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
