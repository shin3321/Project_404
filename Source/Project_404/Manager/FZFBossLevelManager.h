// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FZFBossLevelManager.generated.h"
class AFZFLaserActor;

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
	
	UFUNCTION()
	void HandlePhaseChanged(EBossPhase NewPhase);
	
	UFUNCTION()
	void FirstPhase();
	
	UFUNCTION()
	void Laser();
	
	UFUNCTION()
	void DeactivateLaser(AFZFLaserActor* Laser);

protected:
	FTimerHandle LaserTimerHandle;
	
	UPROPERTY(EditAnywhere, Category = "Gimmick|Laser")
	TSubclassOf<class AFZFLaserActor> LaserClass;
	
	UPROPERTY()
	TArray<TObjectPtr<AFZFLaserActor>> LaserPool;
	
	int32 LaserCount = 20;

public:
	UFUNCTION()
	void SecondPhase();

	UFUNCTION()
	void CreateTrigger();

protected:
	// 트리거 오버랩 시 실행될 함수 (매개변수 형태를 반드시 이렇게 맞춰야 합니다)
	UFUNCTION()
	void OnBombTriggerOverlap(AActor* OverlappedActor, AActor* OtherActor);

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TArray<TObjectPtr<class ATriggerBox>> BombTriggers;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UBoxComponent* TransferVolume;

	int32 MoveSpeed = 300.0f;
	
	FVector MinWallLocation = FVector(0.0f, 0.0f, 0.0f);

	UPROPERTY()
	TArray<AFZFLaserActor*> AvailableLasers;
};
