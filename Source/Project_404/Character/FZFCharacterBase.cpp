#include "Character/FZFCharacterBase.h"

#include "Character/Player/FZFCharacterPlayer.h"
#include "Net/UnrealNetwork.h"
#include "GAS/FZFAbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "GameplayTag/FZFGameplayTags.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/FZFAnimInstance.h"

AFZFCharacterBase::AFZFCharacterBase()
{
	// 네트워크 설정
	bReplicates = true;
	SetReplicateMovement(true);
}

void AFZFCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFZFCharacterBase, bIsDead);
}

UAbilitySystemComponent* AFZFCharacterBase::GetAbilitySystemComponent() const
{
	return ASC;
}

void AFZFCharacterBase::InitAbilitySystem()
{
	// 이 함수 내부에서 InitAbilityActorInfo를 실행하도록 로직 구현
	// Beginplay()에서 함수 호출
}

void AFZFCharacterBase::OnRep_IsDead()
{
	if (bIsDead)
	{
		if (Cast<AFZFCharacterPlayer>(this))
		{

			TriggerDeathGameplayCue();
		}
		else
		{
			ExecuteDeathSequence();
		}
	}
}

void AFZFCharacterBase::TriggerDeathGameplayCue()
{
	if (GetAbilitySystemComponent())
	{
		GetAbilitySystemComponent()->AddLooseGameplayTag(FZFGameplayTags::State_Dead);
		UE_LOG(LogTemp, Warning, TEXT("[Multicast] 모든 클라이언트에서 실행되는 구간 진입"));
		FGameplayCueParameters Parameters;
		Parameters.Location = GetActorLocation();

		UE_LOG(LogTemp, Warning, TEXT("[Multicast] ExecuteGameplayCue 실행 직전"));
		GetAbilitySystemComponent()->ExecuteGameplayCue(FZFGameplayTags::GameplayCue_Character_Death, Parameters);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[Multicast] 에러: ASC가 nullptr입니다"));
	}
}

void AFZFCharacterBase::SetDead()
{
	if (bIsDead)
	{
		return;
	}

	if (!HasAuthority())
	{
		return;
	}

	bIsDead = true;
	if (Cast<AFZFCharacterPlayer>(this))
	{
		return;
	}
	else
	{
		ExecuteDeathSequence();
	}
}

void AFZFCharacterBase::PlayDeadAnimation()
{

	// 몽타주 재생을 위해 애님 인트섵스 가져오기.
	UFZFAnimInstance* AnimInstance = Cast<UFZFAnimInstance>(GetMesh()->GetAnimInstance());
	if (AnimInstance)
	{
		// 재생 중일 수 있는 몽타주 모두 종료.
		AnimInstance->StopAllMontages(0.0f);
		AnimInstance->Montage_Play(DeadMontage);
	}
}

void AFZFCharacterBase::ExecuteDeathSequence()
{
	bIsDead = true;

	// 무브먼트 끄기
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

	// 애니메이션 재생 (이제 서버/클라 모두 실행됨)
	PlayDeadAnimation();

	// GAS 정리
	if (UAbilitySystemComponent* ActiveASC = GetAbilitySystemComponent())
	{
		ActiveASC->CancelAbilities();
		ActiveASC->ClearAllAbilities();
	}

	// 콜리전 끄기
	//SetActorEnableCollision(false);

	// Gameplay Cue 실행 (이미 TriggerDeathGameplayCue가 있다면 여기서
	//TriggerDeathGameplayCue();
}

void AFZFCharacterBase::ServerSetDead_Implementation()
{
	SetDead();
}
