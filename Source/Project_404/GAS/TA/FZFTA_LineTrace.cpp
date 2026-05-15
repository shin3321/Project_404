#include "GAS/TA/FZFTA_LineTrace.h"
#include "Character/Player/FZFPlayerController.h"
#include "Abilities/GameplayAbility.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Physics/FZFCollision.h"
#include "DrawDebugHelpers.h"

AFZFTA_LineTrace::AFZFTA_LineTrace()
{
}

void AFZFTA_LineTrace::ConfirmTargetingAndContinue()
{
	if (SourceActor)
	{
		FGameplayAbilityTargetDataHandle DataHandle = MakeTargetData();
		TargetDataReadyDelegate.Broadcast(DataHandle);
	}
}

void AFZFTA_LineTrace::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);

	// SourceActor = GAS의 AvatarActor : 이 판정을 시작한 주체
	SourceActor = Ability->GetCurrentActorInfo()->AvatarActor.Get();
}

FGameplayAbilityTargetDataHandle AFZFTA_LineTrace::MakeTargetData() const
{
	ACharacter* Character = Cast<ACharacter>(SourceActor);
	FHitResult OutHitResult;
	
	// AttributeSet의 속성으로 변경해야함
	const float AttackRange = 100.0f;
	const float AttackRadius = 50.0f;

	FCollisionQueryParams Params(SCENE_QUERY_STAT(AFZFTA_LineTrace), false, Character);
	const FVector Forward = Character->GetActorForwardVector();
	const FVector Start = Character->GetActorLocation() + Forward * Character->GetCapsuleComponent()->GetScaledCapsuleRadius();
	const FVector End = Start + Forward * AttackRange;

	bool HitDetected = GetWorld()->SweepSingleByChannel(OutHitResult, Start, End, FQuat::Identity, CCHANNEL_FZFATTACK, FCollisionShape::MakeSphere(AttackRadius));

	FGameplayAbilityTargetDataHandle DataHandle;
	if (HitDetected)
	{
		FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit(OutHitResult);
		// Add에서 Shared 포인터를 사용해서 넣어줘서 레퍼런스가 유지되는 한 객체가 유지됨
		DataHandle.Add(TargetData);
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
