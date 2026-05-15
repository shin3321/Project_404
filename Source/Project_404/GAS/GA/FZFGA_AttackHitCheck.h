#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "FZFGA_AttackHitCheck.generated.h"

UCLASS()
class PROJECT_404_API UFZFGA_AttackHitCheck : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UFZFGA_AttackHitCheck();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
