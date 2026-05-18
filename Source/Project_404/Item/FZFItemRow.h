// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include  "Engine/DataTable.h"
#include  "FZFItemRow.generated.h"

USTRUCT(BlueprintType)
struct FFZFItemRow : public  FTableRowBase
{
	GENERATED_BODY()
	
	public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	TSubclassOf<AActor> ItemActorClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	FName ItemId;
};
