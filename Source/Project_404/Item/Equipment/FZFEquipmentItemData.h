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

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    EEquipmentType EquipmentType;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TObjectPtr<UAnimMontage> AttackMontage = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float AttackPower = 0.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float Range = 0.0f;
};
