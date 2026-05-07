// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Monster/FZFMonster.h"
#include "GAS/FZFAbilitySystemComponent.h"
#include "GAS/Attributes/FZFAttributeSet.h"
#include "AI/FZFAIController.h"

AFZFMonster::AFZFMonster()
{
	// Ability System Components 설정
	ASC = CreateDefaultSubobject<UFZFAbilitySystemComponent>("AbilitySystem");
	ASC->SetIsReplicated(true);

	// AIController 클래스 설정.
	AIControllerClass = AFZFAIController::StaticClass();

	// 맵에서 로드 또는 런타임에 스폰(생성)되는 모든 경우
	// 미리 지정한 AIController에 빙의되도록 설정.
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// 몬스터 메시 위치 & 회전 변경
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -88.0f), FRotator(0.0f, -90.0f, 0.0f));

	// 몬스터 메시 설정
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MonsterMesh(
		TEXT("/Game/Jisung/Sci-FI_Troopers_Collection/SciFITrooper-02/SkeletalMesh/SK_SciFiTrooperV2.SK_SciFiTrooperV2")
	);

	if (MonsterMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(MonsterMesh.Object);
	}

}

void AFZFMonster::BeginPlay()
{
	Super::BeginPlay();
	InitAbilitySystem();
}

void AFZFMonster::InitAbilitySystem()
{
	Super::InitAbilitySystem();
	ASC->InitAbilityActorInfo(this, this);

}

float AFZFMonster::GetAIPatrolRadius()
{
	return 800.0f;
}

float AFZFMonster::GetAIDetectRange()
{
	return 400.0f;
}

float AFZFMonster::GetAIAttackRange()
{
	// 공격 거리.
	// 캡슐 형태 = 공격 거리 + (공격 반경 x 2).
	return 0.0f;
}

float AFZFMonster::GetAITurnSpeed()
{
	return 0.0f;
}

void AFZFMonster::AttackByAI()
{
	// 공격 재생.
	//ProcessComboCommand();

	// 공격 끝난 후 처리.
	// Todo: 아직 공격 언제 끝났는지 모름.
}

void AFZFMonster::SetAIAttackDelegate(const FAICharacterAttackFinished& InOnAttackFinished)
{
	// 델리게이트를 변수에 저장.
	OnAttackFinished = InOnAttackFinished;
}
