// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "FZFGA_TogglePickaxe.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_404_API UFZFGA_TogglePickaxe : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
    UFZFGA_TogglePickaxe();

    virtual void ActivateAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData
    ) override;
};
