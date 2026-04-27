// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Equipment/FZFEquipmentItemData.h"
#include "FZFMeleeItemData.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_404_API UFZFMeleeItemData : public UFZFEquipmentItemData
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AttackInterval = 0.5f;
};
