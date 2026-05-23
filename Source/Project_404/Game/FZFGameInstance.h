// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "FZFGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_404_API UFZFGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Game Data")
	void AddStorageItem(const FName ItemId);
	
	UFUNCTION()
	void SetGameResult(bool Result);
public:
	UFUNCTION(BlueprintCallable, Category = "FZFGame")
	bool GetWinGame() { return bWinGame; }

	UPROPERTY(BlueprintReadWrite, Category = "Game Data")
	bool bWinGame = false;

	UPROPERTY(BlueprintReadWrite, Category = "Game Data")
	bool bEnterBossLevel = false;
	
	UPROPERTY(BlueprintReadOnly, Category = "Item Data")
	TArray<FName> StorageItems; 
};
