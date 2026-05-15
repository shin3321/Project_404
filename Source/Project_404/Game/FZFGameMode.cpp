// Fill out your copyright notice in the Description page of Project Settings.


#include "FZFGameMode.h"
#include "Character/Player/FZFPlayerController.h"
#include "Character/Player/FZFPlayerState.h"
#include "Character/FZFCharacterBase.h"

#include "Item/FZFItemBase.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "NavigationSystem.h"

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

	bDelayedStart = false;

	// 레벨이 바뀌어도 정보 유지
	bUseSeamlessTravel = true;

	// 플레이어가 처음 접속할 때 관전자로 스폰될지 여부
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
	UE_LOG(LogTemp, Warning, TEXT("GameMode StartPlay Called End!"));
}

void AFZFGameMode::BeginPlay()
{
	UE_LOG(LogTemp, Log, TEXT("GameMode BeginPlay"));
	Super::BeginPlay();
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

	if (NewPlayer && NewPlayer->GetPawn() == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Pawn이 없습니다. 강제 재시작 시도: %s"), *NewPlayer->GetName());
		RestartPlayer(NewPlayer);
	}

	++CurrentPlayerCount;
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

bool AFZFGameMode::ReadyToStartMatch_Implementation()
{
	if (!Super::ReadyToStartMatch_Implementation()) return false;
	TArray<AActor*> FoundActors;

	// 1. 현재 월드(레벨)의 이름을 가져옵니다.
	FString CurrentLevelName = GetWorld()->GetName();

	// [디버그용] 출력창에 현재 레벨 이름을 찍어봅니다.
	// 주의: FString을 %s로 출력할 때는 반드시 앞에 *를 붙여야 합니다.
	UE_LOG(LogTemp, Warning, TEXT("ReadyToStartMatch 실행 중... 현재 레벨: %s"), *CurrentLevelName);

	// 2. 만약 특정 레벨(예: 전투가 일어나는 메인 게임 레벨)에서만 이 대기 로직을 쓰고 싶다면:
	if (CurrentLevelName.Contains(TEXT("Lobby")))
	{
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), FoundActors);

		if (FoundActors.Num() > 0)
		{
			return true;
		}
		return false;
	}

	return true;
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
}

void AFZFGameMode::StartNewDay_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("A new day has started"));
	GameState->CurrentPhase = EGamePhase::Exploration;
	GetWorldTimerManager().SetTimer(DayTimerHandle, this, &AFZFGameMode::UpdateGameCLock, 1.0f, true);
}

// 아이템 재배치 함수 
// Todo 맵에 Navigation System 배치하기
void AFZFGameMode::NavigateItemRelocate()
{
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (!NavSys) return;

	TArray<AActor*> CurrentItems;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFZFItemBase::StaticClass(), CurrentItems);

	for (AActor* Item : CurrentItems)
	{
		FNavLocation RandomLocation;

		// 특정 중심점 기분으로 반경 유닛 내에서 랜덤한 아이템 배치
		// 중심점 편의상 0,0으로 함
		FVector Origin = FVector::ZeroVector;

		// 반경
		float SearchRadius = 5000.0f;

		if (NavSys->GetRandomReachablePointInRadius(Origin, SearchRadius, RandomLocation))
		{
			Item->SetActorLocation(RandomLocation.Location);
		}
	}
}
