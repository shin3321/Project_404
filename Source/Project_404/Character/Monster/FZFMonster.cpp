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
		TEXT("/Game/Sci-FI_Troopers_Collection/SciFITrooper-02/SkeletalMesh/SK_SciFiTrooperV2.SK_SciFiTrooperV2")
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
