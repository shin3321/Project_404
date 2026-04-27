// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/FZFItemData.h"
#include "Item/ItemTypes.h"
#include "FZFConsumableItemData.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_404_API UFZFConsumableItemData : public UFZFItemData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EConsumableEffectType EquipmentType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Value = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Duration = 0.0f;
};
