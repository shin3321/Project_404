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

	// [중요] GAS 내부 ActorInfo의 SkeletalMeshComponent를 현재 타겟 메시로 일시 변경합니다.
	// 이렇게 해야 PlayMontageAndWait 태스크가 기본 Mesh 대신 ArmMesh 등 우리가 원하는 메시에 애니메이션을 복제/재생합니다.
	FGameplayAbilityActorInfo* MutableActorInfo = const_cast<FGameplayAbilityActorInfo*>(ActorInfo);
	TWeakObjectPtr<USkeletalMeshComponent> OriginalMesh = MutableActorInfo->SkeletalMeshComponent;
	MutableActorInfo->SkeletalMeshComponent = TargetMesh;

	// 기존 Montage_Play와 WaitDelay를 하나로 합친 PlayMontageAndWait 태스크 생성
	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		TEXT("PlayerAttackTask"),
		AttackMontage,
		AttackSpeed,
		NAME_None,
		false // bStopWhenAbilityEnds (어빌리티 종료 시 애니메이션을 멈출지 여부)
	);

	// 태스크 생성이 끝나면 ActorInfo를 원래 상태로 복구해 줍니다 (사후 사이드 이펙트 방지)
	MutableActorInfo->SkeletalMeshComponent = OriginalMesh;

	if (!MontageTask)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 몬스터와 동일하게 애니메이션 상태 델리게이트 바인딩
	MontageTask->OnCompleted.AddDynamic(this, &UFZFGA_Attack::OnMontageCompleted);
	MontageTask->OnInterrupted.AddDynamic(this, &UFZFGA_Attack::OnMontageInterrupted);
	MontageTask->OnCancelled.AddDynamic(this, &UFZFGA_Attack::OnMontageInterrupted);

	// 태스크 활성화 (이 시점에 애니메이션이 재생되고 대기가 시작됩니다)
	MontageTask->ReadyForActivation();
	//const float Duration = TargetMesh->GetAnimInstance()->Montage_Play(AttackMontage, AttackSpeed);

	//if (Duration <= 0.f)
	//{
	//	EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	//	return;
	//}

	//UAbilityTask_WaitDelay* WaitTask = UAbilityTask_WaitDelay::WaitDelay(this, Duration);

	//if (!WaitTask)
	//{
	//	EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	//	return;
	//}

	//WaitTask->OnFinish.AddDynamic(this, &UFZFGA_Attack::OnMontageCompleted);
	//WaitTask->ReadyForActivation();
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
	if (ActorInfo && ActorInfo->AvatarActor.IsValid())
	{
		if (ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get()))
		{
			if (Character->GetCharacterMovement())
			{
				Character->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
			}
		}

		if (AFZFMonster* Monster = Cast<AFZFMonster>(ActorInfo->AvatarActor.Get()))
		{
			Monster->NotifyAttackActionEnd();
		}
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

