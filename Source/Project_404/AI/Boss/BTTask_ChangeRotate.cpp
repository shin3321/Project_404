#include "AI/Boss/BTTask_ChangeRotate.h"

#include "AIController.h"
#include "Interface/FZFBossAIInterface.h"
#include "Components/SkeletalMeshComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/Boss/FZFBossAI.h"
#include "Character/Monster/Boss/FZFBoss.h"

UBTTask_ChangeRotate::UBTTask_ChangeRotate()
{
	NodeName = TEXT("BossRotate");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_ChangeRotate::ExecuteTask(
	UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory
)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}

	APawn* ControllingPawn = AIController->GetPawn();
	if (!ControllingPawn)
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB)
	{
		return EBTNodeResult::Failed;
	}

	float FinalYawOffset = TargetYawOffset;
	float FinalMeshPitch = TargetMeshPitch;

	if (bUseBlackboardValue)
	{
		const bool bUseRotate = BB->GetValueAsBool(BBKEY_USEROTATEBEFOREATTACK);

		if (!bUseRotate)
		{
			return EBTNodeResult::Succeeded;
		}

		FinalYawOffset = BB->GetValueAsFloat(BBKEY_TARGETYAWOFFSET);
		FinalMeshPitch = BB->GetValueAsFloat(BBKEY_TARGETMESHPITCH);
	}

	CachedTargetYaw = FRotator::NormalizeAxis(
		ControllingPawn->GetActorRotation().Yaw + FinalYawOffset
	);

	CachedTargetMeshPitch = FinalMeshPitch;
	return EBTNodeResult::InProgress;
}

void UBTTask_ChangeRotate::TickTask(
	UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory,
	float DeltaSeconds
)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	APawn* ControllingPawn = AIController->GetPawn();
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

	// 1. Actor 전체는 Yaw만 회전
	FRotator CurrentActorRot = ControllingPawn->GetActorRotation();

	FRotator TargetActorRot = CurrentActorRot;
	TargetActorRot.Pitch = 0.f;
	TargetActorRot.Roll = 0.f;
	TargetActorRot.Yaw = CachedTargetYaw;

	FRotator NewActorRot = FMath::RInterpTo(
		CurrentActorRot,
		TargetActorRot,
		DeltaSeconds,
		ActorYawInterpSpeed
	);

	NewActorRot.Pitch = 0.f;
	NewActorRot.Roll = 0.f;

	ControllingPawn->SetActorRotation(NewActorRot);

	// 2. Mesh는 Pitch만 회전
	const float CurrentPitch = Mesh->GetRelativeRotation().Pitch;

	const float NewPitch = FMath::FInterpTo(
		CurrentPitch,
		CachedTargetMeshPitch,
		DeltaSeconds,
		MeshPitchInterpSpeed
	);

	AIPawn->Multicast_SetBossMeshPitch(NewPitch);

	const float PitchDiff = FMath::Abs(FMath::FindDeltaAngleDegrees(
		NewPitch,
		CachedTargetMeshPitch
	));
	UE_LOG(LogTemp, Warning,
		TEXT("[RotateTask] CurrentPitch=%.2f TargetPitch=%.2f NewPitch=%.2f Diff=%.2f"),
		CurrentPitch,
		CachedTargetMeshPitch,
		NewPitch,
		PitchDiff
	);

	const bool bPitchDone = PitchDiff <= PitchTolerance;

	// 테스트용: Pitch 완료 전에는 절대 다음 노드로 못 감
	if (!bPitchDone)
	{
		return;
	}

	const bool bYawDone =
		FMath::Abs(FMath::FindDeltaAngleDegrees(
			NewActorRot.Yaw,
			CachedTargetYaw
		)) <= YawTolerance;

	/*const bool bPitchDone =
		FMath::Abs(FMath::FindDeltaAngleDegrees(
			NewPitch,
			CachedTargetMeshPitch
		)) <= PitchTolerance;*/

	if (bYawDone && bPitchDone)
	{
		FRotator FinalActorRot = ControllingPawn->GetActorRotation();
		FinalActorRot.Pitch = 0.f;
		FinalActorRot.Roll = 0.f;
		FinalActorRot.Yaw = CachedTargetYaw;
		ControllingPawn->SetActorRotation(FinalActorRot);

		// 마지막 정확한 값 보정
		AIPawn->Multicast_SetBossMeshPitchReliable(CachedTargetMeshPitch);


		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}