// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FZFBossLevelManager.generated.h"
class AFZFLaserActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBossBombCreatedEvent, FVector, BombLocation);

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

protected:
	UFUNCTION()
	void OnBombTriggerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
						  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
						  bool bFromSweep, const FHitResult& SweepResult);

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UBoxComponent* TransferVolume;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Laser")
	float MoveSpeed = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Laser")
	FVector MinWallLocation = FVector::ZeroVector;

	UPROPERTY()
	TArray<AFZFLaserActor*> AvailableLasers;
};
