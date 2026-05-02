// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "Manager/FZFRoomManager.h"
#include "FZFGameState.generated.h"

UENUM(BlueprintType)
enum class EGamePhase : uint8
{
	// 탐험 시간
	Exploration,
	// 거점 집결
	Gathering,
	// 하루 종료
	EndDay
};

UCLASS()
class PROJECT_404_API AFZFGameState : public AGameState
{
	GENERATED_BODY()
public:
	AFZFGameState();

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFZFRoomManager* GetRoomManager() const { return RoomManagerComponent; }
protected:
	UPROPERTY()
	UFZFRoomManager* RoomManagerComponent;

public:
	UPROPERTY(Replicated, BlueprintReadOnly)
	int32 CurrentDay = 1;

	UPROPERTY(Replicated, BlueprintReadOnly)
	int32 RemainingRimeSeconds = 420;

	UPROPERTY(Replicated, BlueprintReadOnly)
	EGamePhase CurrentPhase = EGamePhase::Gathering;
};
