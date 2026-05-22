// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Monster/Boss/FZFBoss.h"
#include "GAS/FZFAbilitySystemComponent.h"
#include "GAS/Attributes/FZFMonsterSet.h" // 이거 보스 전용으로 추후 변경
#include "AI/Boss/FZFBossAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayTag/FZFGameplayTags.h"
#include "Components/SkeletalMeshComponent.h"
#include "Character/Monster/MonsterData/FZFBossData.h" // 이거 보스 전용으로 변경
#include "DrawDebugHelpers.h"

AFZFBoss::AFZFBoss()
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
	BossAttributeSet = CreateDefaultSubobject<UFZFMonsterSet>(TEXT("BossAttributeSet")); // Fix: 여기 수정

	// AIController 클래스 설정.
	AIControllerClass = AFZFBossAIController::StaticClass();

	// 맵에서 로드 또는 런타임에 스폰(생성)되는 모든 경우
	// 미리 지정한 AIController에 빙의되도록 설정.
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	RingMeshes.SetNum(3);

	RingMeshes[0] = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Ring01"));
	RingMeshes[1] = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Ring02"));
	RingMeshes[2] = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Ring03"));

	RingMeshes[0]->SetupAttachment(GetMesh());
	RingMeshes[1]->SetupAttachment(GetMesh());
	RingMeshes[2]->SetupAttachment(GetMesh());

	// tick 설정
	PrimaryActorTick.bCanEverTick = true;
}

// 디버깅용 임시 테스트
void AFZFBoss::Tick(float DeltaTime)
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

/* 클래스 멤버 함수(초기화) */
void AFZFBoss::BeginPlay()
{
	Super::BeginPlay();

	bBeginPlayReady = true;

	InitializeBossVisual(); // 서버/클라 둘 다

	InitializeBossServer(); // 서버 전용
}
void AFZFBoss::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	bPossessedReady = true;
	InitializeBossServer();
}

// MonsterData Asset 서버/클라 공통 초기화 (메시/애님/외형)
void AFZFBoss::InitializeBossVisual()
{
	// MonsterData 없으면 return
	if (!BossData)
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] MonsterData is null"), *GetName());
		return;
	}

	// 1. 외형 세팅
	// Seletal Mesh 할당
	GetMesh()->SetSkeletalMesh(BossData->SkeletalMesh);

	// Ring Mesh 할당
	for (int32 i = 0; i < RingMeshes.Num(); ++i)
	{
		ApplyRingData(RingMeshes[i], i);
	}

	// Anim Class 할당
	GetMesh()->SetAnimInstanceClass(BossData->AnimClass);

	// 몽타주 데이터 초기화
	DeadMontage = BossData->MonsterDeadMontage;

	// Mesh Transform 지정 -> BP에서 직접 설정!!! 서버 복제에서 덮어씌워짐!
	// GetMesh()->SetRelativeLocationAndRotation(MonsterData->MeshLocation, MonsterData->MeshRotation);
}

void AFZFBoss::ApplyRingData(USkeletalMeshComponent* RingComp, int32 Index)
{
	if (!RingComp || !BossData) return;

	if (!BossData->RingMeshes.IsValidIndex(Index)) return;

	RingComp->SetSkeletalMesh(BossData->RingMeshes[Index]);

	if (BossData->RingAnim.IsValidIndex(Index) && BossData->RingAnim[Index])
	{
		RingComp->PlayAnimation(BossData->RingAnim[Index], true);
		RingComp->SetPlayRate(BossData->RingSpeed);
	}
}

// MonsterData Asset 서버만 초기화 (GAS/BT/Ability/AI)
void AFZFBoss::InitializeBossServer()
{
	// 서버에서 초기화
	if (!HasAuthority())
	{
		return;
	}

	// MonsterData없으면 return
	if (!BossData)
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
	InitBossEffectClass = BossData->InitAttributeEffect;

	// 나머지 GE 할당
	ChaseSpeedEffectClass = BossData->ChaseSpeedEffect;

	// Ability들 할당
	StartupAbilities = BossData->Abilities;

	// 2. GAS 초기화
	InitAbilitySystem();

	// 3. AttributeSet 값 초기화
	InitAttributesFromData();

	AFZFBossAIController* AIController = Cast<AFZFBossAIController>(GetController());
	if (!AIController || !BossData || !BossData->BehaviorTree)
	{
		return;
	}
	AIController->RunAI();

	// 처음만 초기화 설정 True
	bMonsterInitialized = true;
}

/* GAS 초기세팅 */
void AFZFBoss::InitAbilitySystem()
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
			BossAttributeSet = const_cast<UFZFMonsterSet*>(FoundMonsterSet);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[%s] ASC에서 UFZFBossSet 못 찾음"), *GetName());
		}

		if (BossAttributeSet == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("[%s] BossAttributeSet 로드 실패!"), *GetName());
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

// AttributeSet 초기화
void AFZFBoss::InitAttributesFromData() // 보스 전용을 만들면 AttributeSet 초기화도 수정
{
	if (!ASC || !BossData || !InitBossEffectClass)
	{
		return;
	}

	FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
	Context.AddSourceObject(this);

	FGameplayEffectSpecHandle SpecHandle =
		ASC->MakeOutgoingSpec(InitBossEffectClass, 1.f, Context);

	if (!SpecHandle.IsValid())
	{
		return;
	}

	FGameplayEffectSpec* Spec = SpecHandle.Data.Get();

	Spec->SetSetByCallerMagnitude(FZFGameplayTags::Data_Stat_MaxHp, BossData->MaxHp);
	Spec->SetSetByCallerMagnitude(FZFGameplayTags::Data_Move_MaxMovementSpeed, BossData->MaxMovementSpeed);
	Spec->SetSetByCallerMagnitude(FZFGameplayTags::Data_Attack_MaxAttack, BossData->MaxAttack);
	Spec->SetSetByCallerMagnitude(FZFGameplayTags::Data_Attack_AttackAreaRadius, BossData->AttackAreaRadius);
	Spec->SetSetByCallerMagnitude(FZFGameplayTags::Data_Attack_AttackAreaHalfHeight, BossData->AttackAreaHalfHeight);
	Spec->SetSetByCallerMagnitude(FZFGameplayTags::Data_Attack_AttackSpeed, BossData->AttackSpeed);
	Spec->SetSetByCallerMagnitude(FZFGameplayTags::Data_Attack_PullStrength, BossData->PullStrength);
	Spec->SetSetByCallerMagnitude(FZFGameplayTags::Data_BT_DetectRange, BossData->DetectRange);
	Spec->SetSetByCallerMagnitude(FZFGameplayTags::Data_BT_TurnSpeed, BossData->TurnSpeed);

	FActiveGameplayEffectHandle Handle = ASC->ApplyGameplayEffectSpecToSelf(*Spec);
}

// BT 전달 함수
UBehaviorTree* AFZFBoss::GetBT()
{
	return BossData ? BossData->BehaviorTree : nullptr;
}

// 공격 여부 델리게이트 저장
void AFZFBoss::SetAIAttackDelegate(const FBossAICharacterAttackFinished& InOnAttackFinished)
{
	// 델리게이트를 변수에 저장.
	OnAttackFinished = InOnAttackFinished;

	// 실제 공격 종료 시점은 Gameplay Ability(GA) 내부에서 
	// OnAbilityEnded 델리게이트를 통해 OnAttackFinished.ExecuteIfBound()를 호출
}

void AFZFBoss::AttackByAI()
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
