// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/FZFItemData.h"
#include "FZFCraftMaterialItemData.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_404_API UFZFCraftMaterialItemData : public UFZFItemData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName MaterialTag;
};
