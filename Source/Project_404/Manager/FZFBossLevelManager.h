// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/StaticMeshActor.h"
#include "FZFBossLevelManager.generated.h"
class AFZFLaserActor;
class ALevelSequenceActor;
class ULevelSequencePlayer;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBossBombCreatedEvent, FVector, BombLocation);

// 보스 연출 끝 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBossIntroFinishedEvent);

UCLASS()
class PROJECT_404_API AFZFBossLevelManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFZFBossLevelManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	void StartMapPattern(FName PatternName);

	void StopMapPattern();
	
	/* 맵 공격 패턴 - 레이저 */
	UFUNCTION()
	void Laser();
	
	UFUNCTION()
	void DeactivateLaser(AFZFLaserActor* Laser);

	UPROPERTY(BlueprintAssignable, Category = "Boss|Event")
	FBossBombCreatedEvent OnBossBombCreated;

protected:
	FTimerHandle LaserTimerHandle;
	
	UPROPERTY(EditAnywhere, Category = "Gimmick|Laser")
	TSubclassOf<class AFZFLaserActor> LaserClass;
	
	UPROPERTY()
	TArray<TObjectPtr<AFZFLaserActor>> LaserPool;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Laser")
	int32 LaserPoolCount = 20;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Laser")
	int32 MinSpawnLaserCount = 2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Laser")
	int32 MaxSpawnLaserCount = 6;

	/* 페이즈 변경 - 함정 소환 */
public:

	void OnBossPhaseChanged(int32 NewPhase);

	UFUNCTION()
	void CreateTrigger();
	
	UFUNCTION()
	void CreateBomb(FVector SpawnLocation);

	UPROPERTY(EditAnywhere, Category = "Gimmick|Bomb")
	TSubclassOf<class AFZFBossBombActor> BombBlueprintClass;

	// 인트로
	UFUNCTION()
	void StartBossIntro();

	UPROPERTY(BlueprintAssignable, Category = "Boss|Intro")
	FBossIntroFinishedEvent OnBossIntroFinished;

protected:
	UFUNCTION()
	void OnBombTriggerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
						  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
						  bool bFromSweep, const FHitResult& SweepResult);

	// 보스방 문 열기.
	UFUNCTION()
	void OpenBossDoor();

	// 보스방 문 닫기.
	UFUNCTION()
	void CloseBossDoor();

	//  보스 인트로 재생
	void PlayBossIntro();

	// 보스 인트로 마침
	UFUNCTION()
	void FinishBossIntro();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UBoxComponent* TransferVolume;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Laser")
	float MoveSpeed = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Laser")
	FVector MinWallLocation = FVector::ZeroVector;

	UPROPERTY()
	TArray<AFZFLaserActor*> AvailableLasers;

	// 보스방 문
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Intro")
	TObjectPtr<AStaticMeshActor> BossDoor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Intro")
	TObjectPtr<class ALevelSequenceActor> IntroSequenceActor;

	UPROPERTY(EditInstanceOnly, Category = "Boss|Intro")
	TObjectPtr<AActor> BossIntroDummy;

	bool bIntroStarted = false;
};
