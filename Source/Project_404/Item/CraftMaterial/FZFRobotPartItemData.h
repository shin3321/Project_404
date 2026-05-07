// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/FZFItemData.h"
#include "FZFRobotPartItemData.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_404_API UFZFRobotPartItemData : public UFZFItemData
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName MaterialTag;
};
