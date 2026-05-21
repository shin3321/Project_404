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
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UFZFGA_Attack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!ActorInfo || !CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
	if (!ASC)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	const float AttackSpeed = ASC->GetNumericAttribute(UFZFAttributeSet::GetAttackSpeedAttribute());

	AActor* AvatarActor = ActorInfo->AvatarActor.Get();

	if (AFZFCharacterPlayer* Player = Cast<AFZFCharacterPlayer>(AvatarActor))
	{
		PlayPlayerAttack(Player, AttackSpeed, Handle, ActorInfo, ActivationInfo);
		return;
	}

	if (AFZFMonster* Monster = Cast<AFZFMonster>(AvatarActor))
	{
		PlayMonsterAttack(Monster, AttackSpeed, Handle, ActorInfo, ActivationInfo);
		return;
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
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

void UFZFGA_Attack::PlayPlayerAttack(AFZFCharacterPlayer* CharacterPlayer,float AttackSpeed,const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (!CharacterPlayer || !ActorInfo)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 1인칭 애니메이션 (로컬 전용 - 복제 불필요)
	// 내 화면일 때만, 내 팔 매시에 직접 몽타주를 틀어줍니다.
	if (CharacterPlayer->IsLocallyControlled() && FirstPersonAttackMontage)
	{
		if (USkeletalMeshComponent* ArmMesh = CharacterPlayer->GetArmMesh())
		{
			if (UAnimInstance* AnimInstance = ArmMesh->GetAnimInstance())
			{
				AnimInstance->Montage_Play(FirstPersonAttackMontage, AttackSpeed);
			}
		}
	}

	if (!ThirdPersonAttackMontage)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// GAS 기본 매쉬 (3인칭)에 그대로 태스크를 실행함
	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		TEXT("PlayerAttackTask"),
		ThirdPersonAttackMontage, // 항상 3인칭 몽타주를 넘깁니다.
		AttackSpeed,
		NAME_None,
		false
	);

	if (!MontageTask)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 델리게이트 연결
	MontageTask->OnCompleted.AddDynamic(this, &UFZFGA_Attack::OnMontageCompleted);
	MontageTask->OnInterrupted.AddDynamic(this, &UFZFGA_Attack::OnMontageInterrupted);
	MontageTask->OnCancelled.AddDynamic(this, &UFZFGA_Attack::OnMontageInterrupted);

	// 태스크 활성화 (이때 3인칭 몽타주가 재생되고, 다른 클라이언트들에게도 복제됩니다)
	MontageTask->ReadyForActivation();
}

void UFZFGA_Attack::PlayMonsterAttack(class AFZFMonster* Monster, float AttackSpeed, const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (!Monster || !MonsterAttackMontage)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (Monster->GetCharacterMovement())
	{
		Monster->GetCharacterMovement()->SetMovementMode(MOVE_None);
	}

	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			TEXT("MonsterAttackTask"),
			MonsterAttackMontage,
			AttackSpeed,
			NAME_None,
			false
		);

	if (!MontageTask)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	MontageTask->OnCompleted.AddDynamic(this, &UFZFGA_Attack::OnMontageCompleted);
	MontageTask->OnInterrupted.AddDynamic(this, &UFZFGA_Attack::OnMontageInterrupted);
	MontageTask->OnCancelled.AddDynamic(this, &UFZFGA_Attack::OnMontageInterrupted);

	MontageTask->ReadyForActivation();
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

