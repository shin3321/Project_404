// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA/FZFGA_BossRoomDebuff.h"
#include "AbilitySystemComponent.h"

UFZFGA_BossRoomDebuff::UFZFGA_BossRoomDebuff()
{
	// 인게임 도중 자동으로 발동되어야 하므로 Instancing Policy를 설정합니다.
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	// Net Execution Policy를 Server Only 또는 Local Predicted로 설정 (디버프는 보통 서버 전용이 안전합니다)
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;

	// 태그 트리거 조건은 에디터에서
}

void UFZFGA_BossRoomDebuff::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
	if (ASC && DOTEffectClass && HasAuthority(&ActivationInfo))
	{
		// 1초마다 체력을 깎는 GameplayEffect를 플레이어 자신(Self)에게 부여
		FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
		Context.AddInstigator(ActorInfo->AvatarActor.Get(), ActorInfo->AvatarActor.Get());

		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(DOTEffectClass, 1.0f, Context);
		if (SpecHandle.IsValid())
		{
			ActiveDOTEffectHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
}

void UFZFGA_BossRoomDebuff::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
	if (ASC && ActiveDOTEffectHandle.IsValid() && HasAuthority(&ActivationInfo))
	{
		// 보스방에서 나가거나 어빌리티가 종료되면 체력 감소 버프 제거
		ASC->RemoveActiveGameplayEffect(ActiveDOTEffectHandle);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
