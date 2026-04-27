// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Equipment/FZFEquipmentItemData.h"
#include "FZFThrowableItemData.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_404_API UFZFThrowableItemData : public UFZFEquipmentItemData
{
	GENERATED_BODY()

public: 

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AActor> ThrownActorClass; 
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) 
	float ThrowForce = 1000.f; 
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) 
	float ExplosionRadius = 300.f; 
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) 
	bool bConsumedOnUse = true;
};
