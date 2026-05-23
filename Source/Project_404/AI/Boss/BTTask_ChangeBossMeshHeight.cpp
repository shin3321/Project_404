// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Boss/BTTask_ChangeBossMeshHeight.h"
#include "AIController.h"
#include "AI/FZFAI.h"
#include "Character/Monster/FZFMonster.h"

UBTTask_ChangeBossMeshHeight::UBTTask_ChangeBossMeshHeight()
{
	NodeName = TEXT("ChangeHeight");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_ChangeBossMeshHeight::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	return EBTNodeResult::InProgress;
}

void UBTTask_ChangeBossMeshHeight::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!ControllingPawn)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	// Fix: 보스 다시 구현할 때 인터페이스로 이후 교체 필요!
	AFZFMonster* Boss = Cast<AFZFMonster>(ControllingPawn);
	if (!Boss || !Boss->GetMesh())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	// InterpSpeed <= 0이면 보간이 안 될 수 있음
	if (InterpSpeed <= 0.f)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	FVector Loc = Boss->GetMesh()->GetRelativeLocation();

	Loc.Z = FMath::FInterpTo(
		Loc.Z,
		TargetMeshZ,
		DeltaSeconds,
		InterpSpeed
	);

	Boss->GetMesh()->SetRelativeLocation(Loc);

	if (FMath::Abs(Loc.Z - TargetMeshZ) <= Tolerance)
	{
		Loc.Z = TargetMeshZ;
		Boss->GetMesh()->SetRelativeLocation(Loc);

		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
