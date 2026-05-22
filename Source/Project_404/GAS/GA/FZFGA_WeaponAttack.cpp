#include "GAS/GA/FZFGA_WeaponAttack.h"
#include "AbilitySystemComponent.h"

UFZFGA_WeaponAttack::UFZFGA_WeaponAttack()
{
}

void UFZFGA_WeaponAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
	if (ASC)
	{
		// 공격 시작 시, 쿨타임 태그 검사
		bIsStrongAttack = false;
		if (SoftCooldownTag.IsValid() && !ASC->HasMatchingGameplayTag(SoftCooldownTag))
		{
			bIsStrongAttack = true; // 100% 공격
		}
	}

	// 부모 로직 실행
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UFZFGA_WeaponAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// 애니메이션 끝나고 스킬 종료 시, 강한 공격을 했었다면 쿨타임 적용
	if (bIsStrongAttack)
	{
		UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
		if (ASC && SoftCooldownGEClass)
		{
			FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
			ContextHandle.AddInstigator(ActorInfo->AvatarActor.Get(), ActorInfo->AvatarActor.Get());

			FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(SoftCooldownGEClass, 1.0f, ContextHandle);
			if (SpecHandle.IsValid())
			{
				ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
		}
	}

	// 원래 하려된 종료 로직 실행
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
