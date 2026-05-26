// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/FZFGameState.h"

#include "EditorReimportHandler.h"
#include "Blueprint/UserWidget.h"
#include "Manager/FZFSoundManager.h"
#include "Net/UnrealNetwork.h"
#include "UI/GameResultWidget.h"
#include "Manager/FZFSoundManager.h"

AFZFGameState::AFZFGameState()
{
	
}

void AFZFGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	 
	DOREPLIFETIME(AFZFGameState, CurrentPhase);
	DOREPLIFETIME(AFZFGameState, CurrentDay);
    DOREPLIFETIME(AFZFGameState, RemainingTimeSeconds); 
    DOREPLIFETIME(AFZFGameState, SharedMoney); 
	DOREPLIFETIME(AFZFGameState, bGameOver);
}

void AFZFGameState::OnRep_SharedMoney()
{
	
}

void AFZFGameState::SetGameResult(bool Result)
{
	bIsClear = Result;
	bGameOver = true;

	// 서버(리스닝 서버 호스트)에서도 UI를 띄우기 위해 직접 호출
	if (HasAuthority())
	{
		OnRep_GameOver();
	}
}

void AFZFGameState::OnRep_GameOver() const
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC || !PC->IsLocalController()) return;

	// 위젯 클래스가 블루프린트에서 잘 지정되었는지 확인
	if (GameResultWidgetClass)
	{
		// 1. 위젯 생성
		UGameResultWidget* ResultWidget = CreateWidget<UGameResultWidget>(PC, GameResultWidgetClass);
		if (ResultWidget)
		{
			// 2. 결과 텍스트 변경 ("Clear" 또는 "Fail")
			ResultWidget->UpdateGameResult(bIsClear);

			// 3. 화면에 표시
			ResultWidget->AddToViewport();
		}
	}
}

void AFZFGameState::BeginPlay()
{
	Super::BeginPlay();
	if (GetNetMode() != NM_DedicatedServer)
	{
		if (UGameInstance* GI = GetGameInstance())
		{
			if (UFZFSoundManager* SoundManager = GI->GetSubsystem<UFZFSoundManager>())
			{
				SoundManager->PlayBGM(FName("LobbyLevel"));
			}
		}
	}
}

void AFZFGameState::ChangeGamePhase(EGamePhase NewPhase)
{
	if (HasAuthority())
	{
		CurrentPhase = NewPhase;
		OnRep_CurrentPhase();
	}
}

void AFZFGameState::OnRep_CurrentPhase()
{
	UpdateBGMByPhase(CurrentPhase);
}

void AFZFGameState::UpdateBGMByPhase(EGamePhase Phase)
{
	UFZFSoundManager* SoundManager = GetGameInstance()->GetSubsystem<UFZFSoundManager>();
	if (!SoundManager) return;

	switch (Phase)
	{
	case EGamePhase::Base:
		SoundManager->PlayBGM(FName("Base"));
		break;
	case EGamePhase::BossLevel_1:
		SoundManager->PlayBGM(FName("BossLevel_1"));
		break;
	case EGamePhase::BossLevel_2:
		SoundManager->PlayBGM(FName("BossLevel_2"));
		break;
	}
}