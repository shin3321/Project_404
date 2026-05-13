// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Monster/FZFMonster.h"
#include "GAS/FZFAbilitySystemComponent.h"
#include "GAS/Attributes/FZFMonsterSet.h"
#include "AI/FZFAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayTag/FZFGameplayTags.h"

AFZFMonster::AFZFMonster()
{
	// Ability System Components 설정
	ASC = CreateDefaultSubobject<UFZFAbilitySystemComponent>("AbilitySystem");
	ASC->SetIsReplicated(true);

	// MonsterAttributeSet 설정
	MonsterAttributeSet = CreateDefaultSubobject<UFZFMonsterSet>(TEXT("MonsterAttributeSet"));
	
	// AIController 클래스 설정.
	AIControllerClass = AFZFAIController::StaticClass();

	// 맵에서 로드 또는 런타임에 스폰(생성)되는 모든 경우
	// 미리 지정한 AIController에 빙의되도록 설정.
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// 몬스터 메시 위치 & 회전 변경
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 27.0f), FRotator(0.0f, -90.0f, 0.0f));

	// 몬스터 메시 설정
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MonsterMesh(
		TEXT("/Game/Assets/Monster/M1/SK_M1.SK_M1")
	);

	if (MonsterMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(MonsterMesh.Object);
	}

	// 애님 블루프린트 클래스 정보 지정.
	static ConstructorHelpers::FClassFinder<UAnimInstance> MonsterAnim(
		TEXT("/Game/Project404/Character/Monster/Animation/ABP_M1.ABP_M1_C")
	);

	if (MonsterAnim.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(MonsterAnim.Class);
	}

	// 몽타주 및 액션 데이터 기본 값 설정.
	static ConstructorHelpers::FObjectFinder<UAnimMontage> AttackMontageRef(
		TEXT("/Game/Project404/Character/Monster/Animation/AM_AttackM1.AM_AttackM1")
	);
	if (AttackMontageRef.Succeeded())
	{
		AttackMontage = AttackMontageRef.Object;
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

		if (HasAuthority())
		{
			for (const auto& StartupAbility : StartupAbilities)
			{
				if (StartupAbility)
				{
					FGameplayAbilitySpec StartSpec(StartupAbility);
					ASC->GiveAbility(StartSpec);
				}
			}
		}
	}

}

/* 인터페이스 구현 */

// 정찰 범위
float AFZFMonster::GetAIPatrolRadius()
{
	// 따로 데이터 에셋에서 받아오도록 수정 
	// -> 지금은 하드코딩
	return 800.0f;
}

// 플레이어 감지 범위
float AFZFMonster::GetAIDetectRange()
{
	// GAS AttributeSet에서 수치 가져오기
	/*if (!MonsterAttributeSet)
	{
		UE_LOG(LogTemp, Error, TEXT("MonsterAttributeSet nullptr"));
		return 400.0f;
	}*/

	return MonsterAttributeSet->GetDetectRange();
}

// 공격 사거리
float AFZFMonster::GetAIAttackRange()
{
	return MonsterAttributeSet->GetAttackRange();
}

// 공격 감지 범위
float AFZFMonster::GetAIAttackDetectRange()
{
	// GAS AttributeSet에서 수치 가져오기

	// 공격 거리.
	// 캡슐 형태 = 공격 거리 + (공격 반경 x 2).
	return MonsterAttributeSet->GetAttackRange()
		+ (MonsterAttributeSet->GetAttackRadius() * 2);
}

// 회전 스피드
float AFZFMonster::GetAITurnSpeed()
{
	// GAS AttributeSet에서 수치 가져오기
	return MonsterAttributeSet->GetTurnSpeed();
}

// 공격
void AFZFMonster::AttackByAI()
{
	// GAS 어빌리티 실행 (태그 기반)
	if (ASC)
	{
		ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(FZFGameplayTags::Ability_Action_Attack));
	}
}

void AFZFMonster::SetAIAttackDelegate(const FAICharacterAttackFinished& InOnAttackFinished)
{
	// 델리게이트를 변수에 저장.
	OnAttackFinished = InOnAttackFinished;

	// 실제 공격 종료 시점은 Gameplay Ability(GA) 내부에서
	// 몽타주 종료 섹션 등에 GameplayEvent를 날려 캐릭터가 받게 하거나, 
	// OnAbilityEnded 델리게이트를 통해 OnAttackFinished.ExecuteIfBound()를 호출
}

void AFZFMonster::AttackActionEnd()
{
	// 몽타주 재생이 종료되면 캐릭터 이동을 다시 원상 복구.
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

	// 공격이 끝나면 NotifyComboActionEnd() 호출.
	NotifyAttackActionEnd();
}

void AFZFMonster::NotifyAttackActionEnd()
{
	// 앞서 전달받은 델리게이트 실행.

	if (OnAttackFinished.IsBound())
	{
		OnAttackFinished.Execute();
		OnAttackFinished.Unbind(); // 실행 후 언바인드
	}
}