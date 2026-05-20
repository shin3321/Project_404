#include "GAS/TA/FZFTA_GroundArea.h"
#include "GAS/Attributes/FZFAttributeSet.h" // 본인의 AttributeSet 헤더
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/Character.h"
#include "Physics/FZFCollision.h"
#include "Components/CapsuleComponent.h"
#include "Engine/OverlapResult.h"

AFZFTA_GroundArea::AFZFTA_GroundArea()
{
	StartSocketName = NAME_None;
}

void AFZFTA_GroundArea::ConfirmTargetingAndContinue()
{
	if (SourceActor)
	{
		FGameplayAbilityTargetDataHandle DataHandle = MakeTargetData();
		TargetDataReadyDelegate.Broadcast(DataHandle);
	}
}

void AFZFTA_GroundArea::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);

	// SourceActor = GAS의 AvatarActor : 이 판정을 시작한 주체
	SourceActor = Ability->GetCurrentActorInfo()->AvatarActor.Get();
}

FGameplayAbilityTargetDataHandle AFZFTA_GroundArea::MakeTargetData() const
{
	ACharacter* Character = Cast<ACharacter>(SourceActor);
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(SourceActor);

	// AttributeSet의 속성 대입
	// float AttackRange = ASC->GetNumericAttribute(UFZFAttributeSet::GetAttackRangeAttribute());
	float AttackAreaRadius = ASC->GetNumericAttribute(UFZFAttributeSet::GetAttackAreaRadiusAttribute());
	float AttackAreaHalfHeight = ASC->GetNumericAttribute(UFZFAttributeSet::GetAttackAreaHalfHeightAttribute());

	// 시작 위치(Start)와 방향(Forward) 결정
	FVector Start;

	if (bUseSocket && !StartSocketName.IsNone())
	{
		// 몬스터의 손이나 총구 소켓 위치를 시작점으로 설정
		Start = Character->GetMesh()->GetSocketLocation(StartSocketName);
	}
	else
	{
		// 소켓 미사용 시, 기존 방식 (캐릭터 캡슐)
		Start = Character->GetActorLocation();
		Start.Z -= Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	}

	// 판정 실행
	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(AFZFTA_GroundArea), false, Character);

	// Capsule 형태로 장판 타겟액터
	bool HitDetected = GetWorld()->OverlapMultiByChannel(
		OverlapResults,
		Start,
		FQuat::Identity,
		CCHANNEL_FZFATTACK,
		FCollisionShape::MakeCapsule(AttackAreaRadius, AttackAreaHalfHeight),
		Params
	);

	FGameplayAbilityTargetDataHandle DataHandle;
	TSet<TWeakObjectPtr<AActor>> HitActors;

	if (HitDetected)
	{
		for (const FOverlapResult& Overlap : OverlapResults)
		{
			// 액처 Hit 처리
			AActor* HitActor = Overlap.GetActor();
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

			FHitResult Hit;
			Hit.HitObjectHandle = FActorInstanceHandle(HitActor);
			Hit.Component = Overlap.GetComponent();
			Hit.Location = HitActor->GetActorLocation();
			Hit.ImpactPoint = Hit.Location;

			FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit(Hit);
			// Add에서 Shared 포인터를 사용해서 넣어줘서 레퍼런스가 유지되는 한 객체가 유지됨
			DataHandle.Add(TargetData);
		}
	}

#if ENABLE_DRAW_DEBUG

	if (bShowDebug)
	{
		DrawDebugCapsule(
			GetWorld(),
			Start,
			AttackAreaHalfHeight,
			AttackAreaRadius,
			FQuat::Identity,
			HitDetected ? FColor::Yellow : FColor::Red,
			false,
			5.0f
		);
	}

#endif

	return DataHandle;
}
