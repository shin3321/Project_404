// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Utils/FZFSoundRow.h"
#include "FZFSoundManager.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_404_API UFZFSoundManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UFZFSoundManager();	
	virtual ~UFZFSoundManager()= default;
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	// BGM 재생 함수 (데이터 테이블의 Row Name을 받아 재생)
	UFUNCTION(BlueprintCallable, Category = "FZFAudio")
	void PlayBGM(FName RowName);

	// BGM 정지 함수
	UFUNCTION(BlueprintCallable, Category = "FZFAudio")
	void StopBGM(float FadeOutTime = 1.0f);
	
protected:
	// 로드된 데이터 테이블을 보관할 포인터
	UPROPERTY()
	UDataTable* BGMDataTable;

	// 현재 재생 중인 오디오 컴포넌트 추적용
	UPROPERTY(Transient)
	UAudioComponent* CurrentAudioComponent;
	
};
