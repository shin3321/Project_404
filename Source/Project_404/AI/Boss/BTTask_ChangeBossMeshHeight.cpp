// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Boss/BTTask_ChangeBossMeshHeight.h"
#include "AIController.h"
#include "AI/Boss/FZFBossAI.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Interface/FZFBossAIInterface.h"

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

	IFZFBossAIInterface* AIPawn = Cast<IFZFBossAIInterface>(ControllingPawn);
	if (!AIPawn)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	USkeletalMeshComponent* Mesh = AIPawn->GetBossMesh();
	if (!Mesh)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	// 이동 좌표 정보 가져오기
	const float FinalTargetMeshZ = bUseBlackboardTargetZ ? BB->GetValueAsFloat(BBKEY_MESHTARGETZOFFSET) : TargetMeshZ;

	// InterpSpeed <= 0이면 보간이 안 될 수 있음
	if (InterpSpeed <= 0.f)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	FVector Loc = Mesh->GetRelativeLocation();

	Loc.Z = FMath::FInterpTo(
		Loc.Z,
		FinalTargetMeshZ,
		DeltaSeconds,
		InterpSpeed
	);

	Mesh->SetRelativeLocation(Loc);

	if (FMath::Abs(Loc.Z - FinalTargetMeshZ) <= Tolerance)
	{
		Loc.Z = FinalTargetMeshZ;
		Mesh->SetRelativeLocation(Loc);

		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
