// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/FZFGameState.h"

#include "Manager/FZFSoundManager.h"
#include "Net/UnrealNetwork.h"
#include "Manager/FZFSoundManager.h"

AFZFGameState::AFZFGameState()
{

}

void AFZFGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	 
	DOREPLIFETIME(AFZFGameState, CurrentPhase);
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
