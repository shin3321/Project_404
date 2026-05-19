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

void AFZFCharacterBase::HandleDeath()
{
    if (!HasAuthority() || bIsDead) return;
    
    bIsDead = true;

	OnRep_IsDead(); // 서버에서 사망 처리 시, 클라이언트에서도 즉시 사망 처리 로직 실행 (콜리전 끄기, 애니메이션 재생 등)

    // 상태 정리 (무브먼트 차단 및 어빌리티 정리)
    SetDead();

    //if (GetAbilitySystemComponent())
    //{
    //    // 서버에서 태그 추가 (Loose Tag는 서버에서만 추가해도 GE나 다른 수단으로 동기화되지 않으면 클라이언트는 모를 수 있음)
    //    // 하지만 사망 상태는 bIsDead 복제로 확인 가능
    //    GetAbilitySystemComponent()->AddLooseGameplayTag(FZFGameplayTags::EVENT_CHARACTER_DEATH);        
    //}
    //
}

void AFZFCharacterBase::OnRep_IsDead()
{
    // 클라이언트에서 사망 시 처리할 추가 로직 (예: 콜리전 끄기, 애니메이션, 래그돌 등)
    // HandleDeath()가 서버에서 bIsDead를 true로 만들면 클라이언트에서 이 함수가 호출됨
    if (bIsDead)
    {
        // 클라이언트 측 액터 물리 / 이동 컴포넌트 정리
        if (GetCharacterMovement())
        {
            GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
        }

        // 클라이언트 측 충돌 비활성화
        SetActorEnableCollision(false);
    }
}

void AFZFCharacterBase::TriggerDeathGameplayCue()
{
    if (GetAbilitySystemComponent())
    {
        FGameplayCueParameters Parameters;
        Parameters.Location = GetActorLocation();
         
        // GameplayCueNotify_Actor 또는 Static이 이 태그를 감지하여 몽타주를 재생하게함
        GetAbilitySystemComponent()->ExecuteGameplayCue(FZFGameplayTags::GameplayCue_Character_Death, Parameters);
    }
}

void AFZFCharacterBase::SetDead()
{
	if (!HasAuthority())
	{
		return;
	}

	// 무브먼트 끄기.
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
    
    // GAS 어빌리티 제거 로직 추가
    if (UAbilitySystemComponent* ActiveASC = GetAbilitySystemComponent())
    {
        // 현재 재생 중인 모든 어빌리티를 강제로 취소
        ActiveASC->CancelAbilities();

        // 캐릭터가 가진 모든 어빌리티 권한 영구적 삭제
        ActiveASC->ClearAllAbilities();

        //// 죽었을 경우 태그부여
        //FGameplayTag DeadTag = FGameplayTag::RequestGameplayTag(TEXT("Character.State.Dead"));
        //ActiveASC->AddLooseGameplayTag(DeadTag);
    }

	// 죽는 모션 재생 몽타주 재생
	PlayDeadAnimation();
    TriggerDeathGameplayCue();

	// 콜리전 끄기 -> 충돌 청리 되는 콜리전을 꺼줘야 함
	SetActorEnableCollision(false);

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