#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "FZFGA_Run.generated.h"

UCLASS()
class PROJECT_404_API UFZFGA_Run : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UFZFGA_Run();

public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

};
