#include "Character/FZFCharacterBase.h"
#include "GAS/FZFAbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AFZFCharacterBase::AFZFCharacterBase()
{
    // 네트워크 설정
    bReplicates = true;
    //SetReplicates(true);
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

void AFZFCharacterBase::SetDead()
{
	// 죽음 정리 작업.


	// 무브먼트 끄기.
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

	// 죽는 모션 재생 몽타주 재생
	PlayDeadAnimation();

	// 콜리전 끄기 -> 충돌 청리 되는 콜리전을 꺼줘야 함
	SetActorEnableCollision(false);
}

void AFZFCharacterBase::PlayDeadAnimation()
{
	// 몽타주 재생을 위해 애님 인트섵스 가져오기.
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		// 재생 중일 수 있는 몽타주 모두 종료.
		AnimInstance->StopAllMontages(0.0f);

		// 죽음 몽타주 재생.
		AnimInstance->Montage_Play(DeadMontage);
	}
}