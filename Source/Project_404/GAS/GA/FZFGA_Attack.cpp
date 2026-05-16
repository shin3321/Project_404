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
	if (!CharacterPlayer)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	USkeletalMeshComponent* TargetMesh = nullptr;
	UAnimMontage* AttackMontage = nullptr;

	if (CharacterPlayer->IsLocallyControlled())
	{
		TargetMesh = CharacterPlayer->GetArmMesh();
		AttackMontage = FirstPersonAttackMontage;
	}
	else
	{
		TargetMesh = CharacterPlayer->GetMesh();
		AttackMontage = ThirdPersonAttackMontage;
	}

	if (!TargetMesh || !TargetMesh->GetAnimInstance() || !AttackMontage)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	const float Duration = TargetMesh->GetAnimInstance()->Montage_Play(AttackMontage, AttackSpeed);

	if (Duration <= 0.f)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UAbilityTask_WaitDelay* WaitTask = UAbilityTask_WaitDelay::WaitDelay(this, Duration);

	if (!WaitTask)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	WaitTask->OnFinish.AddDynamic(this, &UFZFGA_Attack::OnMontageCompleted);
	WaitTask->ReadyForActivation();
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

