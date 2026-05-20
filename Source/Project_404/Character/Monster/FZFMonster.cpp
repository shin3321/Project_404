// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Monster/FZFMonster.h"
#include "GAS/FZFAbilitySystemComponent.h"
#include "GAS/Attributes/FZFMonsterSet.h"
#include "AI/FZFAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayTag/FZFGameplayTags.h"
#include "Character/Monster/MonsterData/FZFMonsterData.h"
#include "DrawDebugHelpers.h"

AFZFMonster::AFZFMonster()
{
	/* 서버 설정 */
	bReplicates = true;

	// 움직임 관련 설정
	GetCharacterMovement()->SetIsReplicated(true); // CharacterMovementComponent 자체의 네트워크 이동 동기화 활성화
	SetReplicateMovement(true); // 액터 transform(Location/Rotation 등) 복제
	GetCharacterMovement()->bUseControllerDesiredRotation = false; // Controller 회전 따라가지 않음

	// AI 몬스터/NPC 스타일	
	GetCharacterMovement()->bOrientRotationToMovement = true; // 이동 방향으로 캐릭터 회전
	bUseControllerRotationYaw = false; // Pawn/Character 자체가 Controller Yaw 직접 사용 안함

	// Ability System Components 설정
	ASC = CreateDefaultSubobject<UFZFAbilitySystemComponent>("AbilitySystem");
	ASC->SetIsReplicated(true);

	/* 로직 설정 */
	// MonsterAttributeSet 설정
	MonsterAttributeSet = CreateDefaultSubobject<UFZFMonsterSet>(TEXT("MonsterAttributeSet"));
	
	// AIController 클래스 설정.
	AIControllerClass = AFZFAIController::StaticClass();

	// 맵에서 로드 또는 런타임에 스폰(생성)되는 모든 경우
	// 미리 지정한 AIController에 빙의되도록 설정.
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// tick 설정
	PrimaryActorTick.bCanEverTick = true;
}

void AFZFMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			0.f,
			FColor::Yellow,
			FString::Printf(
				TEXT("ActorYaw %.1f / MeshRelYaw %.1f / MeshWorldYaw %.1f"),
				GetActorRotation().Yaw,
				GetMesh()->GetRelativeRotation().Yaw,
				GetMesh()->GetComponentRotation().Yaw
			)
		);
	}*/
}

void AFZFMonster::BeginPlay() 
{
	Super::BeginPlay();

	bBeginPlayReady = true;

	InitializeMonsterVisual(); // 서버/클라 둘 다

	InitializeMonsterServer(); // 서버 전용
}

void AFZFMonster::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	bPossessedReady = true;
	InitializeMonsterServer();
}

void AFZFMonster::InitializeMonsterVisual()
{
	// MonsterData 없으면 return
	if (!MonsterData)
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] MonsterData is null"), *GetName());
		return;
	}

	// 1. 외형 세팅
	// Seletal Mesh 할당
	GetMesh()->SetSkeletalMesh(MonsterData->SkeletalMesh);

	// Anim Class 할당
	GetMesh()->SetAnimInstanceClass(MonsterData->AnimClass);

	// 몽타주 데이터 초기화

	DeadMontage = MonsterData->MonsterDeadMontage;

	// Mesh Transform 지정 -> BP에서 직접 설정!!! 서버 복제에서 덮어씌워짐!
	// GetMesh()->SetRelativeLocationAndRotation(MonsterData->MeshLocation, MonsterData->MeshRotation);
}

void AFZFMonster::InitializeMonsterServer()
{
	// 서버에서 초기화
	if (!HasAuthority())
	{
		return;
	}

	// MonsterData없으면 return
	if (!MonsterData)
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] MonsterData is null"), *GetName());
		return;
	}

	// 한번 초기화 후 다시 호출되면 return
	if (bMonsterInitialized)
	{
		return;
	}

	// BeginPlay()와 Possess상태 체크 후 몬스터 세팅
	if (!bBeginPlayReady || !bPossessedReady)
	{
		return;
	}

	/* 순서대로 초기화 */

	// 1. GAS 데이터 주입
	// Attribute 할당(Init_GE 할당)
	InitMonsterEffectClass = MonsterData->InitAttributeEffect;

	// 나머지 GE 할당
	ChaseSpeedEffectClass = MonsterData->ChaseSpeedEffect;

	// Ability들 할당
	StartupAbilities = MonsterData->Abilities;

	// 2. GAS 초기화
	InitAbilitySystem();

	// 3. AttributeSet 값 초기화
	InitAttributesFromData();

	AFZFAIController* AIController = Cast<AFZFAIController>(GetController());
	if (!AIController || !MonsterData || !MonsterData->BehaviorTree)
	{
		return;
	}
	AIController->RunAI();

	// 처음만 초기화 설정 True
	bMonsterInitialized = true;
}

/* GAS 초기세팅 */

void AFZFMonster::InitAbilitySystem()
{
	Super::InitAbilitySystem();
	if (ASC)
	{
		// ActorInfo 초기화 (소유자와 아바타 설정)
		ASC->InitAbilityActorInfo(this, this);

		// ASC로부터 MonsterSet을 찾아 캐싱
		const UFZFMonsterSet* FoundMonsterSet = ASC->GetSet<UFZFMonsterSet>();

		if (FoundMonsterSet)
		{
			MonsterAttributeSet = const_cast<UFZFMonsterSet*>(FoundMonsterSet);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[%s] ASC에서 UFZFMonsterSet 못 찾음"), *GetName());
		}

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

// 몬스터 데이터 초기화 함수
void AFZFMonster::InitAttributesFromData()
{
	if (!ASC || !MonsterData || !InitMonsterEffectClass)
	{
		return;
	}

	FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
	Context.AddSourceObject(this);

	FGameplayEffectSpecHandle SpecHandle =
		ASC->MakeOutgoingSpec(InitMonsterEffectClass, 1.f, Context);

	if (!SpecHandle.IsValid())
	{
		return;
	}

	FGameplayEffectSpec* Spec = SpecHandle.Data.Get();

	Spec->SetSetByCallerMagnitude(FZFGameplayTags::Data_Stat_MaxHp, MonsterData->MaxHp);
	Spec->SetSetByCallerMagnitude(FZFGameplayTags::Data_Move_MaxMovementSpeed, MonsterData->MaxMovementSpeed);
	Spec->SetSetByCallerMagnitude(FZFGameplayTags::Data_Attack_MaxAttack, MonsterData->MaxAttack);
	Spec->SetSetByCallerMagnitude(FZFGameplayTags::Data_Attack_AttackRange, MonsterData->AttackRange);
	Spec->SetSetByCallerMagnitude(FZFGameplayTags::Data_Attack_AttackRadius, MonsterData->AttackRadius);
	Spec->SetSetByCallerMagnitude(FZFGameplayTags::Data_Attack_AttackAreaRadius, MonsterData->AttackAreaRadius);
	Spec->SetSetByCallerMagnitude(FZFGameplayTags::Data_Attack_AttackAreaHalfHeight, MonsterData->AttackAreaHalfHeight);
	Spec->SetSetByCallerMagnitude(FZFGameplayTags::Data_Attack_AttackSpeed, MonsterData->AttackSpeed);
	Spec->SetSetByCallerMagnitude(FZFGameplayTags::Data_Attack_PullStrength, MonsterData->PullStrength);
	Spec->SetSetByCallerMagnitude(FZFGameplayTags::Data_BT_DetectRange, MonsterData->DetectRange);
	Spec->SetSetByCallerMagnitude(FZFGameplayTags::Data_BT_TurnSpeed, MonsterData->TurnSpeed);

	FActiveGameplayEffectHandle Handle = ASC->ApplyGameplayEffectSpecToSelf(*Spec);
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
	return MonsterAttributeSet->GetDetectRange();
}

// 공격 사거리
float AFZFMonster::GetAIAttackRange()
{
	if (!MonsterAttributeSet)
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] MonsterAttributeSet is null"), *GetName());
		return 0.f;
	}


	// GAS AttributeSet에서 수치 가져오기
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

// BT 전달 함수
UBehaviorTree* AFZFMonster::GetBT()
{
	return MonsterData ? MonsterData->BehaviorTree : nullptr;
}


// 공격 여부 델리게이트 저장
void AFZFMonster::SetAIAttackDelegate(const FAICharacterAttackFinished& InOnAttackFinished)
{
	// 델리게이트를 변수에 저장.
	OnAttackFinished = InOnAttackFinished;

	// 실제 공격 종료 시점은 Gameplay Ability(GA) 내부에서 
	// OnAbilityEnded 델리게이트를 통해 OnAttackFinished.ExecuteIfBound()를 호출
}

// 공격 실행
void AFZFMonster::AttackByAI()
{
	// 서버 예외처리
	if (!HasAuthority())
	{
		return;
	}

	// GAS 어빌리티 실행 (태그 기반)
	if (ASC)
	{
		ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(FZFGameplayTags::Ability_Action_Attack));
	}
}

void AFZFMonster::SetAIMoveSpeedMode(EFZFAIMoveSpeedMode MoveSpeedMode)
{
	// 서버 예외처리
	if (!HasAuthority())
	{
		return;
	}

	if (!ASC || !MonsterAttributeSet)
	{
		return;
	}

	// 기존 속도 버프 제거
	if (ChaseSpeedEffectHandle.IsValid())
	{
		ASC->RemoveActiveGameplayEffect(ChaseSpeedEffectHandle);
		ChaseSpeedEffectHandle.Invalidate();
	}

	// 추격(Chase)상태 일 때만 이동속도 GE 적용
	if (MoveSpeedMode == EFZFAIMoveSpeedMode::Chase && ChaseSpeedEffectClass)
	{
		FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
		Context.AddSourceObject(this);

		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(ChaseSpeedEffectClass, 1.0f, Context);

		if (SpecHandle.IsValid())
		{
			ChaseSpeedEffectHandle = ASC->BP_ApplyGameplayEffectSpecToSelf(SpecHandle);
		}
	}

	// GE 적용/제거 후 최종 MovementSpeed를 CharacterMovement에 반영
	GetCharacterMovement()->MaxWalkSpeed = MonsterAttributeSet->GetMovementSpeed();
}

/* 클래스 멤버 함수 구현 */
void AFZFMonster::NotifyAttackActionEnd()
{
	// 앞서 전달받은 델리게이트 실행.

	if (OnAttackFinished.IsBound())
	{
		OnAttackFinished.Execute();
		OnAttackFinished.Unbind(); // 실행 후 언바인드
	}
}

// 죽음 처리
void AFZFMonster::SetDead()
{
	// 상위 로직 실행
	Super::SetDead();

	// 타이머를 사용해 일정 시간 대기 후 액터 제거.
	// 레퍼런스(&) LValue Reference(참조): LValue(자리를 차지해야함)
	// 람다 구문
	// [] -> 캡처(Capture) - 람다 함수 본문에서 사용할 정보를 설정.
	// () -> 파라미터 선언.
	// -> 리턴 구문.
	// {} -> 본문
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		FTimerDelegate::CreateLambda(
			[&]() /* -> void */
			{
				Destroy();
			}
		),
		DeadEventDelayTime,
		false
	);

}

// 몽타주 애니메이션 재생.
void AFZFMonster::PlayDeadAnimation()
{
	Super::PlayDeadAnimation();
}
