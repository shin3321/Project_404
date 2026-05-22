// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/FZFGameInstance.h"

void UFZFGameInstance::AddStorageItem(const FName ItemId)
{
	StorageItems.Add(ItemId);
}

void UFZFGameInstance::SetGameResult(bool Result)
{
	bWinGame = Result;
}
