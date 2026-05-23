// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/FZFItemData.h"
#include "Item/ItemTypes.h"
#include "FZFEquipmentItemData.generated.h"

UCLASS()
class PROJECT_404_API UFZFEquipmentItemData : public UFZFItemData
{
	GENERATED_BODY()

public:
    float GetRange() { return Range; }
    EEquipmentType GetItemType() {return EquipmentType;}
    FName GetItemId() { return ItemId; }
    float GetDamage() { return BaseDamage; }
protected:

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EEquipmentType EquipmentType;
    
    UPROPERTY(EditAnywhere, Category = "Stats")
    float BaseDamage;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float Range = 0.0f;
};