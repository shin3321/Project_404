// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Monster/Boss/FZFBoss.h"
#include "GAS/FZFAbilitySystemComponent.h"
#include "GAS/Attributes/FZFMonsterSet.h" // 이거 보스 전용으로 추후 변경
#include "AI/Boss/FZFBossAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayTag/FZFGameplayTags.h"
#include "Abilities/GameplayAbility.h"
#include "Components/SkeletalMeshComponent.h"
#include "Character/Monster/MonsterData/FZFBossData.h" // 이거 보스 전용으로 변경
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/Boss/FZFBossState.h"
#include "AI/Boss/FZFBossAI.h"
#include "Boss/FZFEnergyRelay.h"
#include "Manager/FZFBossLevelManager.h"
#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"

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

	// 4. 동력원 파괴 이벤트 바인딩
	BindEnergyRelayEvents();


	// Fix: 나중에 Intro 연출 후 실행되게 빼야함!
	// 5. BT 실행 
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
			TSet<TSubclassOf<UGameplayAbility>> GrantedAbilityClasses;

			// 기존 공통 Ability 지급
			for (const auto& StartupAbility : StartupAbilities)
			{
				if (StartupAbility &&
					!GrantedAbilityClasses.Contains(StartupAbility))
				{
					ASC->GiveAbility(
						FGameplayAbilitySpec(StartupAbility)
					);

					GrantedAbilityClasses.Add(StartupAbility);
				}
			}

			// SkillList의 Ability 지급
			for (const FBossSkillInfo& Skill : BossData->SkillList)
			{
				if (Skill.AbilityClass &&
					!GrantedAbilityClasses.Contains(Skill.AbilityClass))
				{
					ASC->GiveAbility(
						FGameplayAbilitySpec(Skill.AbilityClass)
					);

					GrantedAbilityClasses.Add(Skill.AbilityClass);
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

void AFZFBoss::BindEnergyRelayEvents()
{
	if (!HasAuthority())
	{
		return;
	}

	for (AFZFEnergyRelay* Relay : EnergyRelays)
	{
		if (!Relay)
		{
			continue;
		}

		Relay->OnRelayDestroyed.AddDynamic(
			this,
			&AFZFBoss::HandleEnergyRelayDestroyed
		);
	}
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

	const FBossSkillInfo* Skill = GetCurrentSelectedSkill();
	if (!Skill)
	{
		NotifyAttackActionEnd();
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Selected AbilityClass: %s"),
		*GetNameSafe(Skill->AbilityClass));

	if (ASC)
	{
		for (const FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
		{
			UE_LOG(LogTemp, Warning, TEXT("Given Ability: %s"),
				*GetNameSafe(Spec.Ability ? Spec.Ability->GetClass() : nullptr));
		}
	}
	
	// 1. 맵 패턴 공격 실행
	if (Skill->bIsMapPattern)
	{
		RequestMapPattern(*Skill);
		return;
	}
	else 
	{
		// 자체 공격 실행
		if (!ASC || !Skill->AbilityClass)
		{
			NotifyAttackActionEnd();
			return;
		}

		// SelfAttack 실행
		// GAS 어빌리티 실행 (클래스 호출)
		const bool bActivated = ASC->TryActivateAbilityByClass(Skill->AbilityClass);
		UE_LOG(LogTemp, Warning, TEXT("Ability Activated: %d"), bActivated);

		if (!bActivated)
		{
			NotifyAttackActionEnd();
		}
	}
}

// 맵 패턴 호출
void AFZFBoss::RequestMapPattern(const FBossSkillInfo& Skill)
{
	if (!HasAuthority())
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("맵 패턴 시작합니다: %s"), *Skill.SkillName.ToString());

	if (!BossLevelManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("BossLevelManager is null"));
		NotifyAttackActionEnd();
		return;
	}

	// 맵 패턴 메니저에게 패턴 시작 요청
	BossLevelManager->StartMapPattern(Skill.SkillName);

	// 정해진 스킬 시간동안 패턴 실행 후 중단되게 시간 설정.
	GetWorld()->GetTimerManager().SetTimer(
		MapPatternTimerHandle,
		this,
		&AFZFBoss::NotifyAttackActionEnd,
		Skill.MapPatternDuration,
		false
	);

	// 기존 스킬 어빌리티나 몽타주 재생 정지는 Waiting 상태로 가서 한번에 정리할거임.
}

// 맵 패턴 중지
void AFZFBoss::StopMapPattern()
{
	// MapPatternManager에게 현재 패턴 정지 요청
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(MapPatternTimerHandle);
	}

	if (BossLevelManager)
	{
		BossLevelManager->StopMapPattern();
	}
}

void AFZFBoss::HandleEnergyRelayDestroyed(AFZFEnergyRelay* Relay)
{
	if (!HasAuthority())
	{
		return;
	}

	if (AFZFBossAIController* BossAI = Cast<AFZFBossAIController>(GetController()))
	{
		if (UBlackboardComponent* BB = BossAI->GetBlackboardComponent())
		{
			BB->SetValueAsEnum(BBKEY_BOSSSTATE, static_cast<uint8>(EBossState::PhaseTransition));
		}
	}
}

void AFZFBoss::ResetBossAction()
{
	UE_LOG(LogTemp, Warning, TEXT("[BossReset] ResetBossAction Called"));

	if (!HasAuthority())
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[BossReset] Finish Attack Task"));
	// 1. 진행 중인 공격 BTTask 먼저 종료.
	NotifyAttackActionEnd();
	
	// 2. 실행 중인 GAS Ability 취소.
	if (ASC)
	{
		UE_LOG(LogTemp, Warning, TEXT("[BossReset] Cancel All Abilities"));
		ASC->CancelAllAbilities();
	}

	// 3. 몽타주 정지
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		UE_LOG(LogTemp, Warning, TEXT("[BossReset] Stop Montage"));
		AnimInstance->Montage_Stop(0.2f);
	}

	// 4. 맵 패턴 정지
	UE_LOG(LogTemp, Warning, TEXT("[BossReset] Stop Map Pattern"));
	StopMapPattern();
}

void AFZFBoss::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFZFBoss, BossPhase);
}

void AFZFBoss::OnRep_BossPhase()
{
	// 서버에서 넘어온 페이즈 값을 바탕으로 연출 실행 (클라이언트)
	OnBossPhaseTransition(BossPhase);
}

// 외부 동력원 델리게이트 전달 함수.
void AFZFBoss::NotifyWaitingStarted()
{
	UE_LOG(LogTemp, Warning, TEXT("[Boss] WaitingStarted Broadcast"));
	OnBossWaitingStarted.Broadcast();
}

void AFZFBoss::NotifyWaitingEnded()
{
	OnBossWaitingEnded.Broadcast();
}

void AFZFBoss::OnBossPhaseTransition(int32 NewPhase)
{
	if (HasAuthority())
	{
		BossPhase = NewPhase;
	}

	// 맨 뒤 배열 고리부터 숨김.
	const int32 RingIndex = RingMeshes.Num() - (NewPhase - 1);

	if (!RingMeshes.IsValidIndex(RingIndex) || !RingMeshes[RingIndex])
	{
		return;
	}

	USkeletalMeshComponent* RingComp = RingMeshes[RingIndex];
	if (!RingComp)
	{
		return;
	}

	const FVector SpawnLocation = RingComp->GetComponentLocation();
	const FRotator SpawnRotation = RingComp->GetComponentRotation();

	// 시각적 연출은 서버/클라이언트 모두에서 수행
	RingComp->SetVisibility(false, true);

	// 파편 스폰은 서버에서만 수행 (복제됨)
	if (HasAuthority())
	{
		if (BrokenRingActorClasses.IsValidIndex(RingIndex) && BrokenRingActorClasses[RingIndex])
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = this;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			AActor* BrokenRing = GetWorld()->SpawnActor<AActor>(
				BrokenRingActorClasses[RingIndex],
				SpawnLocation,
				SpawnRotation,
				SpawnParams
			);

			if (BrokenRing)
			{
				BrokenRing->SetLifeSpan(5.0f);

				if (USkeletalMeshComponent* MeshComp = BrokenRing->FindComponentByClass<USkeletalMeshComponent>())
				{
					MeshComp->SetSimulatePhysics(true);
					MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

					MeshComp->AddImpulse(
						FVector(0.f, 0.f, -300.f),
						NAME_None,
						true
					);
				}
			}
		}

		// 페이즈 변경 시 함정 소환
		if (BossLevelManager)
		{
			BossLevelManager->OnBossPhaseChanged(NewPhase);
		}

		// 보스 죽음 처리
		if (NewPhase >= 4)
		{
			SetDead();
		}
	}
}

/* 클래스 멤버 함수 구현 */
void AFZFBoss::NotifyAttackActionEnd()
{
	UE_LOG(LogTemp, Warning, TEXT("[BossReset] NotifyAttackActionEnd"));

	// 앞서 전달받은 델리게이트 실행.
	if (OnAttackFinished.IsBound())
	{
		UE_LOG(LogTemp, Warning, TEXT("[BossReset] Attack Task Finished"));
		OnAttackFinished.Execute();
		OnAttackFinished.Unbind(); // 실행 후 언바인드
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[BossReset] OnAttackFinished Not Bound"));
	}
}

void AFZFBoss::SetDead()
{
	if (!HasAuthority())
	{
		return;
	}

	// 진행 중인 공격/어빌리티/몽타주/맵패턴 정리
	ResetBossAction();

	// 공격 BTTask 델리게이트 정리
	OnAttackFinished.Unbind();


	// BT 중지
	AFZFBossAIController* AIController = Cast<AFZFBossAIController>(GetController());
	if (AIController)
	{
		AIController->StopAI();
	}

	// 외부 동력원 이벤트 정리
	OnBossWaitingStarted.Clear();
	OnBossWaitingEnded.Clear();

	// 부모 사망 처리: 이동 끄기, 어빌리티 취소/삭제, 충돌 끄기
	Super::SetDead();

	// 사망 몽타주 재생
	PlayDeadAnimation();

	// 일정 시간 뒤 제거
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		[this]()
		{
			Destroy();
		},
		DeadEventDelayTime,
		false
	);
}