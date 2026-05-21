#pragma once
#include "CoreMinimal.h"
#include "FZFStoreItemBase.h"
#include "StorageItemInfo.generated.h"

USTRUCT(BlueprintType)
struct FFZFStorageItem
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
	FName ItemName;

	UPROPERTY(BlueprintReadWrite)
	FVector SpawnLocation;
	
	FFZFStorageItem()
		: ItemName(NAME_None), SpawnLocation(FVector::ZeroVector)
	{}
	
	FFZFStorageItem(FName InItemName, FVector InSpawnLocation)
		:ItemName(InItemName), SpawnLocation(InSpawnLocation)
	{}
};
