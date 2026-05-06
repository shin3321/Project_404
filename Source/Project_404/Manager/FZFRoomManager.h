// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "FZFRoomManager.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_404_API UFZFRoomManager : public UObject
{
	GENERATED_BODY()

public:
	UFZFRoomManager();

	UFUNCTION()
	void SpendMoney(float InMoney)
	{
		Money -= InMoney;
	}

	UFUNCTION()
	void ImportMoney(float InMoney)
	{
		Money += InMoney;
	}

	UFUNCTION()
	void SpendGoods(float InGoods)
	{
		Goods -= InGoods;
	}

	UFUNCTION()
	void ImportGoods(float InGoods)
	{
		Goods += InGoods;
	} 

	// Getter

	UFUNCTION()
	float GetMoney()
	{
		return Money;
	}

private:
	UPROPERTY()
	float Money;
	
	UPROPERTY()
	float Goods;
};
