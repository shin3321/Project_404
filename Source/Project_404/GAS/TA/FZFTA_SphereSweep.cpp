#include "GAS/TA/FZFTA_SphereSweep.h"
#include "GAS/Attributes/FZFAttributeSet.h" // 본인의 AttributeSet 헤더
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/Character.h"
#include "Character/Player/FZFCharacterPlayer.h"
#include "Physics/FZFCollision.h"
#include "Inventory/FZFHeldItemComponent.h"
#include "Components/CapsuleComponent.h"
#include "FZFHeldItemActor.h"


AFZFTA_SphereSweep::AFZFTA_SphereSweep()
{
	StartSocketName = NAME_None;
}

void AFZFTA_SphereSweep::ConfirmTargetingAndContinue()
{
	if (SourceActor)
	{
		FGameplayAbilityTargetDataHandle DataHandle = MakeTargetData();
		TargetDataReadyDelegate.Broadcast(DataHandle);
	}
}

void AFZFTA_SphereSweep::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);

	// SourceActor = GAS의 AvatarActor : 이 판정을 시작한 주체
	SourceActor = Ability->GetCurrentActorInfo()->AvatarActor.Get();
}

FGameplayAbilityTargetDataHandle AFZFTA_SphereSweep::MakeTargetData() const
{
	ACharacter* Character = Cast<ACharacter>(SourceActor);
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(SourceActor);

	// AttributeSet의 속성 대입
	float AttackRange = ASC->GetNumericAttribute(UFZFAttributeSet::GetAttackRangeAttribute());
	float AttackRadius = ASC->GetNumericAttribute(UFZFAttributeSet::GetAttackRadiusAttribute());

	// 시작 위치(Start)와 방향(Forward) 결정
	FVector Start = Character->GetMesh()->GetSocketLocation(StartSocketName);
	FVector Forward = Character->GetActorForwardVector();

	if (bUseSocket && !StartSocketName.IsNone())
	{
		AFZFCharacterPlayer* CharacterPlayer = Cast<AFZFCharacterPlayer>(Character);

		if (CharacterPlayer)
		{
			UFZFHeldItemComponent* HeldItemComponent = CharacterPlayer->GetHeldItemComponent();

			if (HeldItemComponent && HeldItemComponent->GetHeldItemActor())
			{
				const UStaticMeshComponent* ItemMeshComponent = HeldItemComponent->GetHeldItemActor()->GetItemMeshComponent();

				if (ItemMeshComponent && ItemMeshComponent->DoesSocketExist(StartSocketName))
				{
					Start = ItemMeshComponent->GetSocketLocation(StartSocketName);

					FVector CameraLocation;
					FRotator CameraRotation;
					Character->GetController()->GetPlayerViewPoint(CameraLocation, CameraRotation);
					Forward = CameraRotation.Vector();
				}
			}
		}
		else
		{
			Start = Character->GetMesh()->GetSocketLocation(StartSocketName);
		}
	}
	else
	{
		Start = Character->GetActorLocation() + Forward * Character->GetCapsuleComponent()->GetScaledCapsuleRadius();
	}

	FVector End = Start + Forward * AttackRange;

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

	// 판정 실행
	FHitResult OutHitResult;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(AFZFTA_SphereSweep), false, Character);

	bool HitDetected = GetWorld()->SweepSingleByChannel(
		OutHitResult, 
		Start, 
		End,
		FQuat::Identity, 
		AttackChannel,
		FCollisionShape::MakeSphere(AttackRadius),
		Params
	);
	
	FGameplayAbilityTargetDataHandle DataHandle;
	if (HitDetected)
	{
		FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit(OutHitResult);
		// Add에서 Shared 포인터를 사용해서 넣어줘서 레퍼런스가 유지되는 한 객체가 유지됨
		DataHandle.Add(TargetData);
		UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *GetNameSafe(OutHitResult.GetActor()));
		UE_LOG(LogTemp, Warning, TEXT("Hit Comp: %s"), *GetNameSafe(OutHitResult.GetComponent()));
		UE_LOG(LogTemp, Warning, TEXT("Hit Comp Class: %s"), *GetNameSafe(OutHitResult.GetComponent()->GetClass()));
		UE_LOG(LogTemp, Warning, TEXT("Hit Bone: %s"), *OutHitResult.BoneName.ToString());
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
