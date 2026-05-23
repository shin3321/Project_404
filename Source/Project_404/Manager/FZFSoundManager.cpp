// Fill out your copyright notice in the Description page of Project Settings.

#include "Manager/FZFSoundManager.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

UFZFSoundManager::UFZFSoundManager()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> DT_Sound(TEXT("/Game/Project404/Utils/DT_SoundTable"));
	if (DT_Sound.Succeeded())
	{
		SoundDataTable = DT_Sound.Object;
	}
}

void UFZFSoundManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	CurrentAudioComponent = nullptr;
}

void UFZFSoundManager::Deinitialize()
{
	if (CurrentAudioComponent && CurrentAudioComponent->IsPlaying())
	{
		CurrentAudioComponent->Stop();
	}
	Super::Deinitialize();
}

void UFZFSoundManager::PlayBGM(FName RowName)
{
	if (!SoundDataTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("Sound DataTable is NULL"));
		return;
	}
	static const FString ContextString(TEXT("PlayBGM"));
	FFZFSoundRow* SoundRow = SoundDataTable->FindRow<FFZFSoundRow>(RowName, ContextString);
	
	if (SoundRow && SoundRow->SoundAsset)
	{
		if (CurrentAudioComponent && CurrentAudioComponent->IsPlaying())
		{
			// 현재 재생 중인 BGM이 있디면 페이드 아웃 후 파괴
			CurrentAudioComponent->FadeOut(SoundRow->FadeOutTime, 0.0f);
		}
		if (UWorld* World = GetWorld())
		{
			CurrentAudioComponent = UGameplayStatics::CreateSound2D(World, SoundRow->SoundAsset, SoundRow->VolumeMultiplier,1.0f, 0.0f, nullptr, true);
			if (CurrentAudioComponent)
			{
				CurrentAudioComponent->bIsUISound = true;
				CurrentAudioComponent->FadeIn(SoundRow->FadeInTime, SoundRow->VolumeMultiplier);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("BGM Row '%s' not found or Asset is missing!"), *RowName.ToString());
	}
}

void UFZFSoundManager::StopBGM(float FadeOutTime)
{
	if (CurrentAudioComponent && CurrentAudioComponent->IsPlaying())
	{
		CurrentAudioComponent->FadeOut(FadeOutTime, 0.0f);
		CurrentAudioComponent = nullptr;
	}
}

void UFZFSoundManager::PlaySFX(FName RowName)
{
	if (!SoundDataTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("Sound DataTable is NULL"));
		return;
	}

	static const FString ContextString(TEXT("PlaySFX"));
	FFZFSoundRow* SoundRow = SoundDataTable->FindRow<FFZFSoundRow>(RowName, ContextString);

	if (SoundRow && SoundRow->SoundAsset)
	{
		if (UWorld* World = GetWorld())
		{
			UGameplayStatics::PlaySound2D(World, SoundRow->SoundAsset, SoundRow->VolumeMultiplier);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SFX Row '%s' not found or Asset is missing!"), *RowName.ToString());
	}
}

void UFZFSoundManager::PlaySFXAtLocation(FName RowName, FVector Location)
{
	if (!SoundDataTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("Sound DataTable is NULL"));
		return;
	}

	static const FString ContextString(TEXT("PlaySFXAtLocation"));
	FFZFSoundRow* SoundRow = SoundDataTable->FindRow<FFZFSoundRow>(RowName, ContextString);

	if (SoundRow && SoundRow->SoundAsset)
	{
		if (UWorld* World = GetWorld())
		{
			UGameplayStatics::PlaySoundAtLocation(World, SoundRow->SoundAsset, Location, SoundRow->VolumeMultiplier, 1.0f, 0.0f, SoundRow->AttenuationAsset);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SFX Row '%s' not found or Asset is missing!"), *RowName.ToString());
	}
}
