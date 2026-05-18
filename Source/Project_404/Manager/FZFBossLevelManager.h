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

	int32 MoveSpeed = 300.0f;
	
	FVector MinWallLocation = FVector(0.0f, 0.0f, 0.0f);

	UPROPERTY()
	TArray<AFZFLaserActor*> AvailableLasers;
};
