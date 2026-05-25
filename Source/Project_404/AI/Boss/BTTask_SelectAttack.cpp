// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Boss/BTTask_SelectAttack.h"
#include "Interface/FZFBossAIInterface.h"
#include "AIController.h"
#include "Character/Monster/MonsterData/FZFBossData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/Boss/FZFBossAI.h"

UBTTask_SelectAttack::UBTTask_SelectAttack()
{
	NodeName = TEXT("SelectAttack");
}

EBTNodeResult::Type UBTTask_SelectAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	UE_LOG(LogTemp, Warning, TEXT("SelectAttack Start"));

	// Task에서 AI폰에 접근해서 공격하라고 명령 전달.
	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!ControllingPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("SelectAttack Failed: No Pawn"));
		return EBTNodeResult::Failed;
	}

	// 인터페이스로 형변환(다운 캐스팅..).
	IFZFBossAIInterface* AIPawn = Cast<IFZFBossAIInterface>(ControllingPawn);
	if (!AIPawn)
	{
		return EBTNodeResult::Failed;
	}

	// 블랙보드에 접근해서 정보 가져오기
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB)
	{
		return EBTNodeResult::Failed;
	}
	// Boss 데이터 읽어오기
	const UFZFBossData* BossData = AIPawn->GetData();
	if (!BossData)
	{
		return EBTNodeResult::Failed;
	}

	// 외부 동력원 Close 전환
	AIPawn->NotifyWaitingEnded();

	// 스킬 선택
	// 페이즈 여부와 가중치로 하나의 스킬을 선택한다.
	TArray<FBossSkillInfo> Candidates;
	float TotalWeight = 0.f;
	const int32 CurrentPhase = BB->GetValueAsInt(BBKEY_CURRENTPHASE);

	for (const FBossSkillInfo& Skill : BossData->SkillList)
	{
		if (Skill.Weight <= 0.f)
		{
			continue;
		}
		if (Skill.UnlockPhase <= CurrentPhase)
		{
			TotalWeight += Skill.Weight;
			Candidates.Add(Skill);
		}
	}

	if (Candidates.IsEmpty() || TotalWeight <= 0.f)
	{
		UE_LOG(LogTemp, Warning, TEXT("SelectAttack Failed: Candidates=%d TotalWeight=%f Phase=%d"),
			Candidates.Num(), TotalWeight, CurrentPhase);
		return EBTNodeResult::Failed;
	}

	float RandomValue = FMath::FRandRange(0.f, TotalWeight);

	for (const FBossSkillInfo& Skill : Candidates)
	{
		RandomValue -= Skill.Weight;

		if (RandomValue <= 0.f)
		{
			// 선택된 스킬을 저장.
			AIPawn->SetCurrentSelectedSkill(Skill);

			// AttackPos 설정
			BB->SetValueAsVector(BBKEY_ATTACKPOS, Skill.AttackPos);
			BB->SetValueAsFloat(BBKEY_MESHTARGETZOFFSET, Skill.MeshTargetZOffset);
			BB->SetValueAsBool(BBKEY_USEROTATEBEFOREATTACK, Skill.bUseRotateBeforeAttack);
			BB->SetValueAsFloat(BBKEY_TARGETYAWOFFSET, Skill.TargetYawOffset);
			BB->SetValueAsFloat(BBKEY_TARGETMESHPITCH, Skill.TargetMeshPitch);

			return EBTNodeResult::Succeeded;
		}
	}

	return EBTNodeResult::Failed;
}
