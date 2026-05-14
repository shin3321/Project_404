// Fill out your copyright notice in the Description page of Project Settings.


#include "FZFGA_Attack.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "GAS/FZFAbilitySystemComponent.h"
#include "GAS/Attributes/FZFAttributeSet.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interface/FZFMonsterAIInterface.h"
#include "Character/Player/FZFCharacterPlayer.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Character/Monster/FZFMonster.h"

UFZFGA_Attack::UFZFGA_Attack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UFZFGA_Attack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// 유효성 검사: 어빌리티가 정상적으로 시작 가능한지, 몽타주가 할당되어 있는지 확인
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo) || AttackMontage == nullptr)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// AttributeSet 정보 받아오기
	UFZFAbilitySystemComponent* ASC = Cast<UFZFAbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get());

	float AttackSpeed = ASC->GetNumericAttribute(UFZFAttributeSet::GetAttackSpeedAttribute());

	USkeletalMeshComponent* TargetMesh = nullptr;

	if (AFZFCharacterPlayer* CharacterPlayer = Cast<AFZFCharacterPlayer>(ActorInfo->AvatarActor.Get()))
	{
		TargetMesh = CharacterPlayer->GetArmMesh();

		float Duration = 0.f;
		if (TargetMesh && TargetMesh->GetAnimInstance())
		{
			Duration = TargetMesh->GetAnimInstance()->Montage_Play(AttackMontage, AttackSpeed);
		}

		if (Duration > 0.f)
		{
			// 몽타주가 끝날 때까지 기다리는 Task 생성
			UAbilityTask_WaitDelay* WaitTask = UAbilityTask_WaitDelay::WaitDelay(this, Duration);
			WaitTask->OnFinish.AddDynamic(this, &UFZFGA_Attack::OnMontageCompleted);
		}
		else
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		}
	}

	// 몽타주 재생하면 캐릭터 이동을 멈춤
	if (AFZFMonster* Character = Cast<AFZFMonster>(ActorInfo->AvatarActor.Get()))
	{
		Character->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	}

	// 애니메이션 몽타주 재생 태스크 생성
	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		TEXT("AttackTask"),
		AttackMontage,
		AttackSpeed,
		NAME_None, // 일단 몽타주 처음을 재생
		false
	);

	// 델리게이트 연결 (애니메이션이 끝나거나 중단되었을 때의 처리)
	MontageTask->OnCompleted.AddDynamic(this, &UFZFGA_Attack::OnMontageCompleted);
	MontageTask->OnInterrupted.AddDynamic(this, &UFZFGA_Attack::OnMontageInterrupted);
	MontageTask->OnCancelled.AddDynamic(this, &UFZFGA_Attack::OnMontageInterrupted);

	// 태스크 활성화
	MontageTask->ReadyForActivation();
}

void UFZFGA_Attack::OnMontageCompleted()
{
	// 애니메이션이 무사히 끝났으므로 어빌리티 종료
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UFZFGA_Attack::OnMontageInterrupted()
{
	// 애니메이션이 끊겼으므로 어빌리티 종료
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UFZFGA_Attack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// 몽타주 재생이 종료되면 캐릭터 이동을 다시 원상 복구.
	if (ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get()))
	{
		Character->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	}

	if (AFZFMonster* Monster = Cast<AFZFMonster>(ActorInfo->AvatarActor.Get()))
	{
		Monster->NotifyAttackActionEnd();
	}
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

