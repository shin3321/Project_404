// Fill out your copyright notice in the Description page of Project Settings.


#include "FZFGameMode.h"
#include "FZFGameState.h"
#include "Character/Player/FZFPlayerController.h"
#include "Character/Player/FZFPlayerState.h"
#include "Game/FZFGameState.h"
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

void AFZFGameMode::BeginPlay()
{
	Super::BeginPlay();

	// Todo 레벨 선택 시 게임 레벨에서 시작하게 옮겨야 함
	GetWorldTimerManager().SetTimer(DayTimerHandle, this, &AFZFGameMode::UpdateGameCLock, 1.0f, false);
	GameState = Cast<AFZFGameState>(GetGameState<AFZFGameState>());
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

void AFZFGameMode::UpdateGameCLock()
{
	if (!GameState) return;
	if (GameState->RemainingRimeSeconds)
	{
		GameState->RemainingRimeSeconds--;
	}
	else
	{
		if (GameState->CurrentDay < 4)
		{
			GameState->CurrentPhase = EGamePhase::Gathering;
			GameState->CurrentDay++;
		}
		else
		{
			// 4일이 모두 끝났을 때 로직
			GetWorldTimerManager().ClearTimer(DayTimerHandle);
		}
	}
}

void AFZFGameMode::StartNewDay()
{
	GameState->CurrentPhase = EGamePhase::Exploration;
	GameState->RemainingRimeSeconds = 420;
	GetWorldTimerManager().SetTimer(DayTimerHandle, this, &AFZFGameMode::UpdateGameCLock, 1.0f, false);
}
