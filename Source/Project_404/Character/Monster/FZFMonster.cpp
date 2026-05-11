// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Monster/FZFMonster.h"
#include "GAS/FZFAbilitySystemComponent.h"
#include "GAS/Attributes/FZFMonsterSet.h"
#include "AI/FZFAIController.h"

AFZFMonster::AFZFMonster()
{
	// Ability System Components 설정
	ASC = CreateDefaultSubobject<UFZFAbilitySystemComponent>("AbilitySystem");
	ASC->SetIsReplicated(true);

	// MonsterAttributeSet 설정
	CreateDefaultSubobject<UFZFMonsterSet>(TEXT("MonsterAttributeSet"));
	
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
	if (ASC)
	{
		// ActorInfo 초기화 (소유자와 아바타 설정)
		ASC->InitAbilityActorInfo(this, this);

		// ASC로부터 MonsterSet을 찾아 캐싱
		MonsterAttributeSet = const_cast<UFZFMonsterSet*>(ASC->GetSet<UFZFMonsterSet>());
		if (MonsterAttributeSet == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("[%s] MonsterAttributeSet 로드 실패!"), *GetName());
		}
	}

}

float AFZFMonster::GetAIPatrolRadius()
{
	// 인터페이스에서 작성
	return 800.0f;
}

float AFZFMonster::GetAIDetectRange()
{
	// GAS AttributeSet에서 수치 가져오기
	MonsterAttributeSet->GetDetectRange();
	return 400.0f;
}

float AFZFMonster::GetAIAttackRange()
{
	// GAS AttributeSet에서 수치 가져오기
	MonsterAttributeSet->GetAttackRange();
	// 공격 거리.
	// 캡슐 형태 = 공격 거리 + (공격 반경 x 2).
	return 0.0f;
}

float AFZFMonster::GetAITurnSpeed()
{
	// GAS AttributeSet에서 수치 가져오기
	return 0.0f;
}

void AFZFMonster::AttackByAI()
{
	//if (ASC)
	//{
	//	// 공격 어빌리티 실행 (태그 기반)
	//	// "Character.Action.Attack" 등의 태그를 미리 지정해두거나 변수로 관리
	//	FGameplayTag AttackTag = FGameplayTag::RequestGameplayTag(FName("Character.Action.Attack"));
	//	ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(AttackTag));
	//}
	
	// 공격 재생.
	//ProcessComboCommand();

	// 공격 끝난 후 처리.
	// Todo: 아직 공격 언제 끝났는지 모름.
}

void AFZFMonster::SetAIAttackDelegate(const FAICharacterAttackFinished& InOnAttackFinished)
{
	// 델리게이트를 변수에 저장.
	OnAttackFinished = InOnAttackFinished;

	// 실제 공격 종료 시점은 Gameplay Ability(GA) 내부에서
	// 몽타주 종료 섹션 등에 GameplayEvent를 날려 캐릭터가 받게 하거나, 
	// OnAbilityEnded 델리게이트를 통해 OnAttackFinished.ExecuteIfBound()를 호출
}
