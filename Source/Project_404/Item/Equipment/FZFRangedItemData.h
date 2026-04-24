// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Equipment/FZFEquipmentItemData.h"
#include "FZFRangedItemData.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_404_API UFZFRangedItemData : public UFZFEquipmentItemData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AttackInterval = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AActor> ProjectileClass;
};
