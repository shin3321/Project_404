// Fill out your copyright notice in the Description page of Project Settings.


#include "FZFBossGameMode.h"
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
}

void AFZFBossGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AFZFBossGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}
