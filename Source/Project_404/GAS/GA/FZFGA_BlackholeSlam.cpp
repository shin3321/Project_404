// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA/FZFGA_BlackholeSlam.h"

UFZFGA_BlackholeSlam::UFZFGA_BlackholeSlam()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UFZFGA_BlackholeSlam::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

}

void UFZFGA_BlackholeSlam::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
}

void UFZFGA_BlackholeSlam::OnPullFinished()
{
}

void UFZFGA_BlackholeSlam::OnSlamHitEventReceived(FGameplayEventData Payload)
{
}

void UFZFGA_BlackholeSlam::OnMontageCompleted()
{
}