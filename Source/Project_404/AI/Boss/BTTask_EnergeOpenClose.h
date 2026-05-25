// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_EnergeOpenClose.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_404_API UBTTask_EnergeOpenClose : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_EnergeOpenClose();

protected:
	virtual EBTNodeResult::Type ExecuteTask(
		UBehaviorTreeComponent& OwnerComp,
		uint8* NodeMemory
	) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsOnEnergy;
	
};
