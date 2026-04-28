// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/FZFCharacterBase.h"
#include "FZFMonster.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_404_API AFZFMonster : public AFZFCharacterBase
{
	GENERATED_BODY()

public:
	AFZFMonster();

protected:
	virtual void BeginPlay() override;

	virtual void InitAbilitySystem() override;

	
};
