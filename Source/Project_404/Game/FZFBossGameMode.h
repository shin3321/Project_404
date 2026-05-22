// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/FZFGameMode.h"
#include "FZFBossGameMode.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_404_API AFZFBossGameMode : public AFZFGameMode
{
	GENERATED_BODY()

public:
	AFZFBossGameMode();

protected:
	UFUNCTION()
	virtual void BeginPlay() override;
	UFUNCTION()
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
public:
	UFUNCTION()
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION()	
	void DeadPlayer();
	
	UFUNCTION()	
	void BossGameRule();

protected:
	int32 ExistPlayers;
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Management")
	TSubclassOf<class AFZFBossLevelManager> BossLevelManagerClass;
};
