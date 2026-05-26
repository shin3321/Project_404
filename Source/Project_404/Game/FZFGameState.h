// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "Blueprint/UserWidget.h"
#include "FZFGameState.generated.h"

UENUM(BlueprintType)
enum class EGamePhase : uint8
{
	MainLevel	UMETA(DisplayName = "메인 레벨"),
	Base		UMETA(DisplayName = "거점"),
	InGame		UMETA(DisplayName = "InGame State"),
	BossLevel_1	UMETA(DisplayName = "Boss Phase1"),
	BossLevel_2	UMETA(DisplayName = "Boss Phase2")
};

UCLASS()
class PROJECT_404_API AFZFGameState : public AGameState
{
	GENERATED_BODY()
public:
	AFZFGameState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
protected:
	virtual void BeginPlay() override;
	
public:	
	EGamePhase GetCurrentPhase() const {return CurrentPhase;}
	
	UPROPERTY(ReplicatedUsing = OnRep_CurrentPhase, BlueprintReadOnly, Category = "Game Phase")
	EGamePhase CurrentPhase;
	
	// 서버가 이 값을 변경하는 함수
	void ChangeGamePhase(EGamePhase NewPhase);
	
	UFUNCTION()
	void OnRep_CurrentPhase();
	
	// 내부적으로 BGM을 교체하는 로직
	void UpdateBGMByPhase(EGamePhase Phase);
	
	UPROPERTY(ReplicatedUsing=OnRep_SharedMoney, Transient)
	int32 SharedMoney;

	UFUNCTION()
	void OnRep_SharedMoney();
	
	UFUNCTION()
	void SetGameResult(bool Result);
	
	UPROPERTY(BlueprintReadOnly, Category = "GameRules")
	bool bIsClear = false;

	UPROPERTY(ReplicatedUsing=OnRep_GameOver, BlueprintReadOnly, Category = "GameRules")
	bool bGameOver = false;

	UFUNCTION()
	void OnRep_GameOver() const;
	
private:	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UGameResultWidget> GameResultWidgetClass;
	
public:
	UPROPERTY(Replicated, BlueprintReadOnly)
	int32 CurrentDay = 1;

	UPROPERTY(Replicated, BlueprintReadOnly)
	int32 RemainingTimeSeconds = 1200;	
	
	
};
