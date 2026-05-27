// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item/ItemTypes.h"
#include "FZFSpawnManager.generated.h"

USTRUCT(BlueprintType)
struct FFZFItemSpawnConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AFZFItemBase> ItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SpawnCount = 1;
};

USTRUCT(BlueprintType)
struct FFZFMonsterSpawnConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AFZFMonster> MonsterClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SpawnCount = 1;
};

UCLASS()
class PROJECT_404_API AFZFSpawnManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFZFSpawnManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:	 
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSpawnItem(FName ItemId, FVector SpawnLocation, FRotator SpawnRotation);

private:
	UPROPERTY(EditAnywhere, Category = "Item")
	TArray<FFZFItemSpawnConfig> WeaponPartSpawnConfigs;

	UPROPERTY(EditAnywhere, Category = "Item")
	TArray<FFZFItemSpawnConfig> RobotPartSpawnConfigs;

	UPROPERTY(EditAnywhere, Category = "Monster")
	TArray<FFZFMonsterSpawnConfig> MonsterSpawnConfigs;

	UPROPERTY(EditAnywhere, Category = "ItemTable")
	UDataTable* ItemTable;
};
