// Fill out your copyright notice in the Description page of Project Settings.


#include "Utils/Boss/FZFLaserActor.h"

// Sets default values
AFZFLaserActor::AFZFLaserActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CurrentMode = ELaserMode::Inactive;
}

// Called when the game starts or when spawned
void AFZFLaserActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AFZFLaserActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (CurrentMode != ELaserMode::Moving)
		return;
	FVector NewLocation = GetActorLocation();

	if (CurrentType == ELaserType::Virtical)
	{
		NewLocation.Z += Speed * MoveDirection.Z * DeltaTime;
		if (NewLocation.Z < MinZ || NewLocation.Z > MaxZ)
		{
			MoveDirection.Z *= -1.0;
		}
	}
	if (CurrentType == ELaserType::Horizon)
	{
		NewLocation.Y += Speed * MoveDirection.Y * DeltaTime;
		if (NewLocation.Y < 0.0f || NewLocation.Y > MaxY)
			MoveDirection.Y *= -1.0;
	}
	SetActorLocation(NewLocation);
	if (NewLocation.X < MinX || NewLocation.X > MaxX)
	{
		DeactivateLaser();
	}
}

void AFZFLaserActor::ActivateLaser(FVector StartLocation, ELaserMode Mode, ELaserType Type, float MoveSpeed)
{
	SetActorHiddenInGame(false);
	SetActorLocation(StartLocation);
	CurrentMode = Mode;
	Speed = MoveSpeed;
	CurrentType = Type;
}

void AFZFLaserActor::DeactivateLaser()
{
	CurrentMode = ELaserMode::Inactive;
	SetActorHiddenInGame(true);
}

ELaserMode AFZFLaserActor::GetLaserMode()
{
	return CurrentMode;
}
