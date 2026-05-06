// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/FZFGameState.h"
#include "Net/UnrealNetwork.h"

AFZFGameState::AFZFGameState()
{

}

void AFZFGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFZFGameState, CurrentDay);
	DOREPLIFETIME(AFZFGameState, RemainingRimeSeconds);
	DOREPLIFETIME(AFZFGameState, CurrentPhase);
}
