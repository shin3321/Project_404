// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_ChangeMoveSpeed.h"

UBTTask_ChangeMoveSpeed::UBTTask_ChangeMoveSpeed()
{
	NodeName = TEXT("ChangeMoveSpeed");

}

EBTNodeResult::Type UBTTask_ChangeMoveSpeed::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);



	return EBTNodeResult::Type();
}
