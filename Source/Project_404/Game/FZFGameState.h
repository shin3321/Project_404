// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "Manager/FZFRoomManager.h"
#include "FZFGameState.generated.h"

/**
 * 
 */
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
};
