// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TimerHandle.h"
#include "GameFramework/GameMode.h"

#include "FZFGameMode.generated.h"

/**
 *
 */
UCLASS()
class PROJECT_404_API AFZFGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AFZFGameMode();

protected:
	virtual void BeginPlay() override;

	virtual void HandleMatchHasStarted() override;
public:
	virtual void StartPlay() override;
	// 접속 및 스폰 관리 로직
	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	virtual void RestartPlayer(AController* NewPlayer) override;

	virtual bool ReadyToStartMatch_Implementation() override;
	//void LoadLevelAsync();
	//void OnLevelLoaded();
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "GameEvent")
	void StartNewDay();
	
public:
	UFUNCTION(BlueprintCallable, Category = "Network")
	void OnAllPlayersReady();

	UFUNCTION()
	void BossDefeated();

	virtual void PlayerDied(AController* DeadPlayer);

	virtual void HandleGameOver();

protected:
	class AFZFGameState* GameState;

	UPROPERTY()
	FTimerHandle DayTimerHandle;
	void UpdateGameCLock();

	void MoveToLobby();
	
	UPROPERTY(EditDefaultsOnly, Category = "GameRules")
	float TravelDelayTime = 3.0f;
	
protected:
	// 현재 접속 중인 플레이어 수A
	int32 CurrentPlayerCount = 0;

	// 최대 허용 인원 
	int32 MaxPlayers = 4;
	
	bool bIsGameOver = false;
};
