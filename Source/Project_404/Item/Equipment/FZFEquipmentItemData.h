// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/FZFItemData.h"
#include "Item/ItemTypes.h"
#include "FZFEquipmentItemData.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_404_API UFZFEquipmentItemData : public UFZFItemData
{
	GENERATED_BODY()

protected:

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EEquipmentType EquipmentType;
        
    // GAS 관련 (핵심)
    // 이 무기를 들었을 때 부여할 Ability
    UPROPERTY(EditAnywhere, Category = "GAS") 
    TSubclassOf<class UGameplayAbility> WeaponAbilityClass; 
    
    // 이 무기가 타겟에게 적용할 효과 (데미지, 기절 등) 
    UPROPERTY(EditAnywhere, Category = "GAS")
    TSubclassOf<class UGameplayEffect> DamageEffectClass;
    
    UPROPERTY(EditAnywhere, Category = "Stats")
    float BaseDamage;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float Range = 0.0f;
};