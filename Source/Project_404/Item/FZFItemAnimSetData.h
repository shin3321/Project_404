// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FZFItemAnimSetData.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_404_API UFZFItemAnimSetData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ThirdPerson")
    TObjectPtr<UAnimSequence> ThirdPersonIdle;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FirstPerson")
    TObjectPtr<UAnimSequence> FirstPersonIdle;
};
