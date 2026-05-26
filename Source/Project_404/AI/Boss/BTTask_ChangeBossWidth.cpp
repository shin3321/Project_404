// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Boss/BTTask_ChangeBossWidth.h"
#include "AIController.h"
#include "AI/Boss/FZFBossAI.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Interface/FZFBossAIInterface.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UBTTask_ChangeBossWidth::UBTTask_ChangeBossWidth()
{
	NodeName = TEXT("ChangeWidth");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_ChangeBossWidth::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	return EBTNodeResult::InProgress;
}

void UBTTask_ChangeBossWidth::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!ControllingPawn)
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
	ACharacter* Character = Cast<ACharacter>(ControllingPawn);
	if (!Character)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	// 회전 정지 코드
	Character->GetCharacterMovement()->bOrientRotationToMovement = false;

	// 이동 좌표 정보 가져오기
	const float FinalTargetY = bUseBlackboardTargetY ? BB->GetValueAsVector(BBKEY_TARGETPOS).Y : TargetMeshY;

	UE_LOG(LogTemp, Display, TEXT("TargetPos 정보: %f"), BB->GetValueAsVector(BBKEY_TARGETPOS).Y);

	// InterpSpeed <= 0이면 보간이 안 될 수 있음
	if (InterpSpeed <= 0.f)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	FVector Loc = ControllingPawn->GetActorLocation();

	Loc.Y = FMath::FInterpTo(
		Loc.Y,
		FinalTargetY,
		DeltaSeconds,
		InterpSpeed
	);

	ControllingPawn->SetActorLocation(Loc, true);

	if (FMath::Abs(Loc.Y - FinalTargetY) <= Tolerance)
	{
		Loc.Y = FinalTargetY;
		ControllingPawn->SetActorLocation(Loc, true);

		// 회전 작동 코드
		Character->GetCharacterMovement()->bOrientRotationToMovement = true;
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
