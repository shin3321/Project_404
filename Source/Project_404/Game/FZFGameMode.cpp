// Fill out your copyright notice in the Description page of Project Settings.


#include "FZFGameMode.h"
#include "Character/Player/FZFPlayerController.h"
#include "Character/Player/FZFPlayerState.h"
#include "Character/FZFCharacterBase.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"

#include "Game/FZFGameState.h"
#include "Project_404.h"


AFZFGameMode::AFZFGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> DefaultPawnClassRef(TEXT("/Game/Project404/Character/Player/BP_FZFPlayer.BP_FZFPlayer_C"));
	if (DefaultPawnClassRef.Succeeded())
	{
		DefaultPawnClass = DefaultPawnClassRef.Class;
		SpectatorClass = DefaultPawnClassRef.Class;
	}

	PlayerControllerClass = AFZFPlayerController::StaticClass();
	GameStateClass = AFZFGameState::StaticClass();
	PlayerStateClass = AFZFPlayerState::StaticClass();

	bDelayedStart = false;

	// 플레이어가 처음 접속할 때 관전자로 스폰될지 여부 (반드시 false)
	bStartPlayersAsSpectators = false;
}

void AFZFGameMode::StartPlay()
{
	UE_LOG(LogTemp, Warning, TEXT("GameMode StartPlay Called!"));

	if (GetWorld()->HasBegunPlay())
	{
		UE_LOG(LogTemp, Warning, TEXT("World says BeginPlay is DONE. But why no log?"));
	}
	Super::StartPlay();
	BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("GameMode StartPlay Called End!"));
}

void AFZFGameMode::BeginPlay()
{
	UE_LOG(LogTemp, Log, TEXT("GameMode BeginPlay"));
	Super::BeginPlay();
	GameState = Cast<AFZFGameState>(GetGameState<AFZFGameState>());

	// Todo 레벨 선택 시 게임 레벨에서 시작하게 옮겨야 함
	GetWorldTimerManager().SetTimer(DayTimerHandle, this, &AFZFGameMode::UpdateGameCLock, 1.0f, true);
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
	++CurrentPlayerCount;

	Super::PostLogin(NewPlayer);

	UE_LOG(LogTemp, Log, TEXT("Player Logged In. Total: %d"),
		CurrentPlayerCount);
}

AActor* AFZFGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	TArray<AActor*> FoundActors;
	// 시작 위치 설정
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), FoundActors);

	if (FoundActors.Num() > 0)
	{
		int32 StartIndex = CurrentPlayerCount % FoundActors.Num();
		AActor* ChosenStart = FoundActors[StartIndex];

		if (IsValid(ChosenStart))
		{
			UE_LOG(LogTemp, Log, TEXT("ChoosePlayerStart: Found %d starts. Assigning Index % d to % s"), FoundActors.Num(), StartIndex, *Player->GetName());
			return ChosenStart;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("ChoosePlayerStart: Falling back to default (Super)"));

	return Super::ChoosePlayerStart_Implementation(Player);
}

void AFZFGameMode::RestartPlayer(AController* NewPlayer)
{
	UE_LOG(LogTemp, Warning, TEXT("RestartPlayer called: %s"), *GetNameSafe(NewPlayer));

	Super::RestartPlayer(NewPlayer);

	UE_LOG(LogTemp, Warning, TEXT("Pawn: %s"), *GetNameSafe(NewPlayer->GetPawn()));
}

void AFZFGameMode::OnAllPlayersReady()
{
	auto CurrentState = GetMatchState();
	UE_LOG(LogTemp, Log, TEXT("CurrentState: %s"), *CurrentState.ToString());
	if (GetMatchState() == MatchState::WaitingToStart)
	{
		StartMatch();
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
	GetWorldTimerManager().SetTimer(DayTimerHandle, this, &AFZFGameMode::UpdateGameCLock, 1.0f, true);
}
