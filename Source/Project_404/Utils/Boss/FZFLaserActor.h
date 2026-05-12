// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FZFLaserActor.generated.h"

UENUM(BlueprintType)
enum class ELaserMode : uint8
{
	Inactive,
	Fixed,
	Moving
};

UENUM(BlueprintType)
enum class ELaserType : uint8
{
	Horizon,
	Virtical
};
UCLASS()
class PROJECT_404_API AFZFLaserActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFZFLaserActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	// 레이저 활성화 함수
	void ActivateLaser(FVector StartLocation, ELaserMode Mode, ELaserType Type, float MoveSpeed);
	
	// 레이저 비활성화 함수
	void DeactivateLaser();
	
	// Getter
	ELaserMode GetLaserMode();
	
private:
	ELaserMode CurrentMode;
	ELaserType CurrentType;
	float Speed;
	FVector MoveDirection;
	
	float MinZ;
	float MaxZ;
	float MinY;
	float MaxY;
	float MinX;
	float MaxX;
};
