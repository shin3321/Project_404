// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item/ItemTypes.h"
#include "FZFSpawnManager.generated.h"

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
	void ServerSpawnItem(FName ItemId, FVector SpawnLocation);
		
	
private:
	UPROPERTY(EditAnywhere, Category = "Item")
	TArray<TSubclassOf<class AFZFItemBase>> ItemClasses;
	
	UPROPERTY(EditAnywhere, Category = "Monster")
	TArray<TSubclassOf<class AFZFMonster>> MonsterClasses;

	UPROPERTY(EditAnywhere, Category = "ItemTable")
	UDataTable* ItemTable;
};
