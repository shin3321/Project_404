// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/FZFLobbyGameMode.h"
#include "manager/FZFSpawnManager.h"

AFZFLobbyGameMode::AFZFLobbyGameMode()
{
}

void AFZFLobbyGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	if (SpawnManagerClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Instigator = GetInstigator();
		SpawnParams.Owner = this;

		AFZFSpawnManager* SpawnManager = GetWorld()->SpawnActor<AFZFSpawnManager>(
			SpawnManagerClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

		if (SpawnManager)
		{
			UE_LOG(LogTemp, Warning, TEXT("서버: 스폰 매니저가 성공적으로 동적 생성되었습니다."));
		}
	}
	
}

void AFZFLobbyGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AFZFLobbyGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}
