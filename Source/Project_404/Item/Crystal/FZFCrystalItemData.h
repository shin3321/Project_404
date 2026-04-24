// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/FZFItemData.h"
#include "FZFCrystalItemData.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_404_API UFZFCrystalItemData : public UFZFItemData
{
	GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    int32 MinWorth = 0;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    int32 MaxWorth = 0;
};
