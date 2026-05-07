// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Item/CraftMaterial/FZFCraftPartItemData.h"
#include "Item/Equipment/FZFEquipmentItemData.h"
#include "FZFEquipmentRecipeData.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_404_API UFZFEquipmentRecipeData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "Base Part")
	TObjectPtr<UFZFCraftPartItemData> BasePart;

	UPROPERTY(EditAnywhere, Category = "Core Part")
	TObjectPtr<UFZFCraftPartItemData> CorePart;

	UPROPERTY(EditAnywhere, Category = "Craft")
	TObjectPtr<UFZFEquipmentItemData> ResultItem;
};
