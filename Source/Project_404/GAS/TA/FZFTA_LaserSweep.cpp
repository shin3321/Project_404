#include "GAS/TA/FZFTA_LaserSweep.h"
#include "GAS/Attributes/FZFAttributeSet.h" // 본인의 AttributeSet 헤더
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/Character.h"
#include "Character/Monster/FZFMonster.h"
#include "Character/Monster/Boss/FZFBoss.h"
#include "Character/Player/FZFCharacterPlayer.h"
#include "GAS/Attributes/FZFMonsterSet.h"
#include "Physics/FZFCollision.h"
#include "Components/CapsuleComponent.h"
#include "Inventory/FZFHeldItemComponent.h"
#include "FZFHeldItemActor.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

AFZFTA_LaserSweep::AFZFTA_LaserSweep()
{
	StartSocketName = NAME_None;
}

void AFZFTA_LaserSweep::ConfirmTargetingAndContinue()
{
	if (SourceActor)
	{
		FGameplayAbilityTargetDataHandle DataHandle = MakeTargetData();
		TargetDataReadyDelegate.Broadcast(DataHandle);
	}
}

void AFZFTA_LaserSweep::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);

	// SourceActor = GAS의 AvatarActor : 이 판정을 시작한 주체
	SourceActor = Ability->GetCurrentActorInfo()->AvatarActor.Get();
}

FGameplayAbilityTargetDataHandle AFZFTA_LaserSweep::MakeTargetData() const
{
	ACharacter* Character = Cast<ACharacter>(SourceActor);
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(SourceActor);

	if (!Character || !ASC) return FGameplayAbilityTargetDataHandle();

	// 시전자가 플레이어인지 검사 (보스/몬스터와 명확하게 분기하기 위함)
	bool bIsPlayer = SourceActor && SourceActor->IsA<AFZFCharacterPlayer>();

	// AttributeSet의 속성 대입
	float AttackRange = 0.0f; 
	float AttackRadius = 0.0f; 
	
	FVector Start = FVector::ZeroVector;
	FVector Forward = FVector::ZeroVector;

	if (bIsPlayer)
	{
		AttackRange = ASC->GetNumericAttribute(UFZFAttributeSet::GetAttackRangeAttribute());
		AttackRadius = ASC->GetNumericAttribute(UFZFAttributeSet::GetAttackRadiusAttribute());
	}
	else if (AFZFBoss* Boss = Cast<AFZFBoss>(Character))
	{
		// 보스 : 보스 전용 MonsterSet 검사
		if (Boss->GetBossAttributeSet())
		{
			AttackRange = Boss->GetBossAttributeSet()->GetAttackRange();
			AttackRadius = Boss->GetBossAttributeSet()->GetAttackRadius();
		}

		// 백업 : 에셋 데이터
		if (AttackRange <= 0.0f || AttackRadius <= 0.0f)
		{
			if (UFZFBossData* BData = Boss->GetData())
			{
				AttackRange = BData->AttackRange;
				AttackRadius = BData->AttackRadius;
			}
		}
	}
	else if (AFZFMonster* Monster = Cast<AFZFMonster>(Character))
	{
		// 일반 몬스터가 가진 ASC에서 직접 UFZFMonsterSet 클래스 찾아 스탯을 긁어옴
		if(const UFZFMonsterSet* MSet = ASC->GetSet<UFZFMonsterSet>())
		{
			AttackRange = MSet->GetAttackRange();
			AttackRadius = MSet->GetAttackRadius();
		}
		// 백업 : 기존 일반 몬스터 데이터 에셋 구조 유지
		if (AttackRange <= 0.0f || AttackRadius <= 0.0f)
		{
			if(UFZFMonsterData* MData = Monster->GetMonsterData())
			{
				AttackRange = MData->AttackRange;
				AttackRadius = MData->AttackRadius;
			}
		}

	}

	UE_LOG(LogTemp, Error, TEXT("[LaserRangeDebug] ========================================================"));
	UE_LOG(LogTemp, Error, TEXT("[LaserRangeDebug] 시전자: %s"), *Character->GetName());
	UE_LOG(LogTemp, Error, TEXT("[LaserRangeDebug] 어트리뷰트 스탯 -> 사거리(Range): %.2f / 반경(Radius): %.2f"), AttackRange, AttackRadius);

	// 실시간 시작 위치와 방향 결정
	if (bIsPlayer)
	{
		// 조준 방향(Rotation)을 가장 확실하게 가져오는 방법
		FRotator AimRotation = FRotator::ZeroRotator;
		if (Character->GetController())
		{
			// 로컬 플레이어나 서버에서 직접 조종 중일 때
			AimRotation = Character->GetController()->GetControlRotation();
		}
		else
		{
			// 멀티플레이에서 다른 클라이언트의 캐릭터(Simulated Proxy)일 때
			AimRotation = Character->GetBaseAimRotation();
		}
		
		if (bUseSocket && !StartSocketName.IsNone())
		{
			// 플레이어라면 카메라/조준 시점(ViewPoint)을 기준으로 정확한 시작점 계산
			FVector ViewLocation;
			FRotator ViewRotation;

			if (Character->GetController())
			{
				Character->GetController()->GetPlayerViewPoint(ViewLocation, ViewRotation);
				Start = ViewLocation;
				Forward = ViewRotation.Vector();
			}
			else
			{
				// 컨트롤러가 없는 경우(다른 플레이어 화면) 소켓 위치 사용
				Start = Character->GetMesh()->GetSocketLocation(StartSocketName);
				Forward = AimRotation.Vector();
			}
		}
		else
		{
			// 소켓 미사용 시 캐릭터 발밑이 아닌 몸통 중간 지점에서 시작하도록 보정
			Start = Character->GetActorLocation() + FVector(0, 0, Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 0.5f);
			Forward = AimRotation.Vector();
		}
	}
	else
	{
		// 보스 / AI 몬스터 전용 조준 로직 추가
		if (bUseSocket && !StartSocketName.IsNone())
		{
			// 보스 데이터 에셋에 설정된 눈 소켓(AttackSocket) 위치를 정확하게 시작점으로 계산
			Start = Character->GetMesh()->GetSocketLocation(StartSocketName);

			// 보스의 몸통 회전이 아닌, 눈 소켓(소켓 창에 배치한 뼈대 방향)의 정면 방향을 획득
			Forward = Character->GetActorForwardVector();

			UE_LOG(LogTemp, Warning, TEXT("[LaserRangeDebug] 보스 소켓 기반 판정 -> 소켓명: %s"), *StartSocketName.ToString());
		}
		else
		{
			// 소켓 미지정 시 보스의 정면 벡터 활용
			Forward = Character->GetActorForwardVector();
			Start = Character->GetActorLocation() + Forward * Character->GetCapsuleComponent()->GetScaledCapsuleRadius();

			UE_LOG(LogTemp, Warning, TEXT("[LaserRangeDebug] 보스 캡슐/정면 기반 판정 (소켓 없음)"));
		}
	}

	// 끝점 계산
	FVector End = Start + Forward * AttackRange;

	UE_LOG(LogTemp, Warning, TEXT("[LaserRangeDebug] 최종 계산된 판정 범위:"));
	UE_LOG(LogTemp, Warning, TEXT("[LaserRangeDebug] -> 시작점(Start) : %s"), *Start.ToString());
	UE_LOG(LogTemp, Warning, TEXT("[LaserRangeDebug] -> 방향(Forward) : %s (길이: %.2f)"), *Forward.ToString(), Forward.Size());
	UE_LOG(LogTemp, Warning, TEXT("[LaserRangeDebug] -> 끝점(End)     : %s"), *End.ToString());
	UE_LOG(LogTemp, Warning, TEXT("[LaserRangeDebug] -> 총 직선 거리  : %.2f"), FVector::Distance(Start, End));

	// 시각적 끝점 찾기 (환경 오브젝트 포함)
	FHitResult VisualHit;
	FCollisionQueryParams VisualParams(SCENE_QUERY_STAT(AFZFTA_LaserVisual), false, Character);
	GetWorld()->LineTraceSingleByChannel(VisualHit, Start, End, ECC_Visibility, VisualParams);
	FVector ActualVisualEnd = VisualHit.bBlockingHit ? VisualHit.ImpactPoint : End;

	UE_LOG(LogTemp, Log, TEXT("[LaserRangeDebug] 환경(벽/바닥) 레이트레이스 결과 -> BlockingHit: %d / 부딪힌 좌표: %s"), VisualHit.bBlockingHit, *ActualVisualEnd.ToString());

	// 실제 데미지 판정 (기존 Sweep)
	// 공격 주체에 따른 트레이스 채널 동적 결정
	ECollisionChannel AttackChannel = bIsPlayer ? CCHANNEL_FZFPLAYER_ATTACK : CCHANNEL_FZFMONSTER_ATTACK;

	TArray<FHitResult> HitResults;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(AFZFTA_LaserSweep), false, Character);

	bool HitDetected = GetWorld()->SweepMultiByChannel(
		HitResults,
		Start,
		End,
		FQuat::Identity,
		AttackChannel,
		FCollisionShape::MakeSphere(AttackRadius),
		Params
	);

	UE_LOG(LogTemp, Log, TEXT("[LaserRangeDebug] 플레이어 대상 Sweep 결과 -> HitDetected: %d / 감지된 타겟 수: %d"), HitDetected, HitResults.Num());

	FGameplayAbilityTargetDataHandle DataHandle;
	TSet<TWeakObjectPtr<AActor>> HitActors;

	if (HitDetected)
	{
		for (const FHitResult& Hit : HitResults)
		{
			AActor* HitActor = Hit.GetActor();
			if (!HitActor || !Cast<APawn>(HitActor) || HitActors.Contains(HitActor))
			{
				continue;
			}

			HitActors.Add(HitActor);
			FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit(Hit);
			DataHandle.Add(TargetData);
		}
	}

	// 만약 적을 아무도 맞추지 못했다면, 시각 효과를 위한 데이터를 강제로 추가
	if (DataHandle.Num() == 0)
	{
		FHitResult DummyHit;
		DummyHit.bBlockingHit = true; // 히트된 것으로 간주
		DummyHit.ImpactPoint = ActualVisualEnd;
		DummyHit.Location = ActualVisualEnd;
		DummyHit.TraceStart = Start;
		DummyHit.TraceEnd = End;
		
		FGameplayAbilityTargetData_SingleTargetHit* VisualData = new FGameplayAbilityTargetData_SingleTargetHit(DummyHit);
		DataHandle.Add(VisualData);
	}

#if ENABLE_DRAW_DEBUG

	if (bShowDebug)
	{
		if (Forward.IsNearlyZero())
		{
			UE_LOG(LogTemp, Error, TEXT("[LaserRangeDebug] 에러: Forward가 0이라 DrawDebugCapsule 회전 행렬을 만들 수 없습니다!"));
		}

		FVector CapsuleOrigin = Start + (End - Start) * 0.5f;
		float CapsuleHalfHeight = AttackRange * 0.5f;
		FColor DrawColor = HitDetected ? FColor::Green : FColor::Red;
		DrawDebugCapsule(GetWorld(), CapsuleOrigin, CapsuleHalfHeight, AttackRadius, FRotationMatrix::MakeFromZ(Forward).ToQuat(), DrawColor, false, 2.0f);
	}

#endif

	return DataHandle;
}
