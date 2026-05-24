#include "GAS/TA/FZFTA_LaserSweep.h"
#include "GAS/Attributes/FZFAttributeSet.h" // 본인의 AttributeSet 헤더
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/Character.h"
#include "Character/Player/FZFCharacterPlayer.h"
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

	// AttributeSet의 속성 대입
	float AttackRange = ASC->GetNumericAttribute(UFZFAttributeSet::GetAttackRangeAttribute());
	float AttackRadius = ASC->GetNumericAttribute(UFZFAttributeSet::GetAttackRadiusAttribute());

	// 시작 위치(Start)와 방향(Forward) 결정
	FVector Start = FVector::ZeroVector;
	
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
	FVector Forward = AimRotation.Vector();

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
		}
	}
	else
	{
		// 소켓 미사용 시 캐릭터 발밑이 아닌 몸통 중간 지점에서 시작하도록 보정
		Start = Character->GetActorLocation() + FVector(0, 0, Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 0.5f);
	}

	FVector End = Start + Forward * AttackRange;

	// 1. 시각적 끝점 찾기 (환경 오브젝트 포함)
	FHitResult VisualHit;
	FCollisionQueryParams VisualParams(SCENE_QUERY_STAT(AFZFTA_LaserVisual), false, Character);
	GetWorld()->LineTraceSingleByChannel(VisualHit, Start, End, ECC_Visibility, VisualParams);
	FVector ActualVisualEnd = VisualHit.bBlockingHit ? VisualHit.ImpactPoint : End;

	// 2. 실제 데미지 판정 (기존 Sweep)
	// 공격 주체에 따른 트레이스 채널 동적 결정
	ECollisionChannel AttackChannel = CCHANNEL_FZFPLAYER_ATTACK; // 매크로에 정의된 기본값을 Player로 설정

	// 만약 플레이어 클래스로 캐스팅이 성공한다면 플레이어 공격 채널 사용
	if (SourceActor && SourceActor->IsA<AFZFCharacterPlayer>())
	{
		AttackChannel = CCHANNEL_FZFPLAYER_ATTACK; // 플레이어 공격 채널
	}
	else
	{
		AttackChannel = CCHANNEL_FZFMONSTER_ATTACK; // 그 외(몬스터 등)는 몬스터 공격 채널
	}

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

	// 3. 만약 적을 아무도 맞추지 못했다면, 시각 효과를 위한 데이터를 강제로 추가
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
		FVector CapsuleOrigin = Start + (End - Start) * 0.5f;
		float CapsuleHalfHeight = AttackRange * 0.5f;
		FColor DrawColor = HitDetected ? FColor::Green : FColor::Red;
		DrawDebugCapsule(GetWorld(), CapsuleOrigin, CapsuleHalfHeight, AttackRadius, FRotationMatrix::MakeFromZ(Forward).ToQuat(), DrawColor, false, 5.0f);
	}

#endif

	return DataHandle;
}
