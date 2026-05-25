#include "AI/Boss/BTTask_ChangeRotate.h"

#include "AIController.h"
#include "Interface/FZFBossAIInterface.h"
#include "Components/SkeletalMeshComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/Boss/FZFBossAI.h"

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
	FRotator CurrentMeshRot = Mesh->GetRelativeRotation();

	FRotator TargetMeshRot = CurrentMeshRot;
	TargetMeshRot.Pitch = CachedTargetMeshPitch;

	FRotator NewMeshRot = FMath::RInterpTo(
		CurrentMeshRot,
		TargetMeshRot,
		DeltaSeconds,
		MeshPitchInterpSpeed
	);

	Mesh->SetRelativeRotation(NewMeshRot);

	const bool bYawDone =
		FMath::Abs(FMath::FindDeltaAngleDegrees(
			NewActorRot.Yaw,
			CachedTargetYaw
		)) <= YawTolerance;

	const bool bPitchDone =
		FMath::Abs(FMath::FindDeltaAngleDegrees(
			NewMeshRot.Pitch,
			CachedTargetMeshPitch
		)) <= PitchTolerance;

	if (bYawDone && bPitchDone)
	{
		FRotator FinalActorRot = ControllingPawn->GetActorRotation();
		FinalActorRot.Pitch = 0.f;
		FinalActorRot.Roll = 0.f;
		FinalActorRot.Yaw = CachedTargetYaw;
		ControllingPawn->SetActorRotation(FinalActorRot);

		FRotator FinalMeshRot = Mesh->GetRelativeRotation();
		FinalMeshRot.Pitch = CachedTargetMeshPitch;
		Mesh->SetRelativeRotation(FinalMeshRot);

		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}