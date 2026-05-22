// Fill out your copyright notice in the Description page of Project Settings.


#include "FZFBossGameMode.h"

#include "Character/Player/FZFCharacterPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "Manager/FZFBossLevelManager.h"

AFZFBossGameMode::AFZFBossGameMode()
{
}

void AFZFBossGameMode::BeginPlay()
{
	Super::BeginPlay();
	if (BossLevelManagerClass)
	{		
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;

		GetWorld()->SpawnActor<AFZFBossLevelManager>(
			BossLevelManagerClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	}
	
	// 현재 플레이어 모두 저장
	TArray<AActor*> ActorList;
	UGameplayStatics::GetAllActorsOfClass(this, AFZFCharacterPlayer::StaticClass(), ActorList);
	ExistPlayers = ActorList.Num();
}

void AFZFBossGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AFZFBossGameMode::DeadPlayer()
{
	ExistPlayers -= 1;
}

void AFZFBossGameMode::BossGameRule()
{
	
}

void AFZFBossGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}
