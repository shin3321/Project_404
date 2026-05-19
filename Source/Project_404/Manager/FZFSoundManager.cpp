// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/FZFSoundManager.h"

UFZFSoundManager::UFZFSoundManager()
{
	static ConstructorHelpers::FObjectFinder<UDataTable>DT_Sound(TEXT("/Game/Project404/Utils/DT_SoundTable"));
	if (DT_Sound.Succeeded())
	{
		BGMDataTable = DT_Sound.Object;
	}
}

void UFZFSoundManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UFZFSoundManager::Deinitialize()
{
	Super::Deinitialize();
}

void UFZFSoundManager::PlayBGM(FName RowName)
{
}

void UFZFSoundManager::StopBGM(float FadeOutTime)
{
}
