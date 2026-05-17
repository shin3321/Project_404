#include "GAS/TA/FZFTA_LaserSweep.h"
#include "GAS/Attributes/FZFAttributeSet.h" // 본인의 AttributeSet 헤더
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/Character.h"
#include "Physics/FZFCollision.h"
#include "Components/CapsuleComponent.h"

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

	// AttributeSet의 속성 대입
	float AttackRange = ASC->GetNumericAttribute(UFZFAttributeSet::GetAttackRangeAttribute());
	float AttackRadius = ASC->GetNumericAttribute(UFZFAttributeSet::GetAttackRadiusAttribute());

	// 시작 위치(Start)와 방향(Forward) 결정
	FVector Start;
	FVector Forward = Character->GetActorForwardVector();

	if (bUseSocket && !StartSocketName.IsNone())
	{
		// 몬스터의 손이나 총구 소켓 위치를 시작점으로 설정
		Start = Character->GetMesh()->GetSocketLocation(StartSocketName);
	}
	else
	{
		// 소켓 미사용 시, 기존 방식 (캐릭터 캡슐)
		Start = Character->GetActorLocation() + Forward * Character->GetCapsuleComponent()->GetScaledCapsuleRadius();
	}

	const FVector End = Start + Forward * AttackRange;

	// 판정 실행
	TArray<FHitResult> HitResults;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(AFZFTA_LaserSweep), false, Character);

	bool HitDetected = GetWorld()->SweepMultiByChannel(
		HitResults,
		Start,
		End,
		FQuat::Identity,
		CCHANNEL_FZFATTACK,
		FCollisionShape::MakeSphere(AttackRadius),
		Params
	);

	FGameplayAbilityTargetDataHandle DataHandle;

	TSet<TWeakObjectPtr<AActor>> HitActors;

	if (HitDetected)
	{
		for (const FHitResult& Hit : HitResults)
		{
			// 액처 Hit 처리
			AActor* HitActor = Hit.GetActor();
			if (!HitActor)
			{
				continue;
			}

			// 캐릭터 Hit 처리
			APawn* HitPawn = Cast<APawn>(HitActor);
			if (!HitPawn)
			{
				continue;
			}

			if (HitActors.Contains(HitActor))
			{
				continue;
			}
			HitActors.Add(HitActor);

			FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit(Hit);
			// Add에서 Shared 포인터를 사용해서 넣어줘서 레퍼런스가 유지되는 한 객체가 유지됨
			DataHandle.Add(TargetData);
		}
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
