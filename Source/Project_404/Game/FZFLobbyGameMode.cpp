// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/FZFLobbyGameMode.h"
#include "Manager/FZFSpawnManager.h"

AFZFLobbyGameMode::AFZFLobbyGameMode()
{
}

void AFZFLobbyGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	// 이미 월드에 스폰 매니저가 있는지 확인 (중복 생성 방지)
	if (UGameplayStatics::GetActorOfClass(GetWorld(), AFZFSpawnManager::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("서버: 월드에 이미 스폰 매니저가 존재합니다. 추가 생성을 건너뜁니다."));
		return;
	}

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
