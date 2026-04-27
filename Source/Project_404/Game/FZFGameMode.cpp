// Fill out your copyright notice in the Description page of Project Settings.


#include "FZFGameMode.h"
#include "FZFGameState.h"
#include "Character/Player/FZFPlayerController.h"
#include "Character/Player/FZFPlayerState.h"
#include "Project_404.h"


AFZFGameMode::AFZFGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> DefaultPawnClassRef(TEXT("/Game/Project404/Character/Player/BP_FZFPlayer.BP_FZFPlayer_C"));
	if (DefaultPawnClassRef.Succeeded())
	{
		DefaultPawnClass = DefaultPawnClassRef.Class;
	}

	PlayerControllerClass = AFZFPlayerController::StaticClass();
	GameStateClass = AFZFGameState::StaticClass();
	PlayerStateClass = AFZFPlayerState::StaticClass();
}

void AFZFGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);

	if (CurrentPlayerCount >= MaxPlayers)
	{
		//ErrorMessage = TEXT("Server is Full");
		UE_LOG(LogTemp, Log, TEXT("Server is Full"));
	}

}

void AFZFGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	++CurrentPlayerCount;

	AFZFGameState* FZFGameState = GetGameState<AFZFGameState>();
	if (FZFGameState)
	{
		//FZFGameState->Multicast_BroadcastMessage(TEXT(""));
	}
}
