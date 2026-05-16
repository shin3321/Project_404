// Fill out your copyright notice in the Description page of Project Settings.


#include "Utils/Boss/FZFLaserActor.h"
#include "Components/BoxComponent.h"
#include "NiagaraComponent.h"

// Sets default values
AFZFLaserActor::AFZFLaserActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CurrentMode = ELaserMode::Inactive;
	
	RootComp = CreateDefaultSubobject<USceneComponent>("RootComponent");
	RootComponent = RootComponent;
	CollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComp"));
	CollisionComp->SetupAttachment(RootComp);
	
	CollisionComp->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	CollisionComp->SetGenerateOverlapEvents(true);
	
	//
	CollisionComp->SetBoxExtent(FVector(500.f, 10.f, 10.f));
	
	LaserEffectComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("LaserEffectComp"));
	LaserEffectComp->SetupAttachment(CollisionComp); // 콜리전을 따라다니도록 설정
	LaserEffectComp->bAutoActivate = false;

	MoveDirection = FVector(1.0f, 1.0f, 1.0f);
	
	//CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AFZFLaserActor::OnLaserOverlap);
	
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
	if (CurrentMode == ELaserMode::Inactive)
		return;
	FVector NewLocation = GetActorLocation();

	if (CurrentType == ELaserType::Horizon && CurrentMode == ELaserMode::Moving)
	{
		NewLocation.Z += Speed * MoveDirection.Z * DeltaTime;
		if (NewLocation.Z < MinZ || NewLocation.Z > MaxZ)
		{
			MoveDirection.Z *= -1.0;
		}
	}
	if (CurrentType == ELaserType::Vertical)
	{
		NewLocation.X += Speed * MoveDirection.X * DeltaTime;
		if (NewLocation.X < 0.0f || NewLocation.X> MaxX)
			MoveDirection.X *= -1.0;
	}

	NewLocation.Y+= Speed * MoveDirection.Y * DeltaTime;
	SetActorLocation(NewLocation);

	if (NewLocation.Y < MinY || NewLocation.Y > MaxY)
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
	
	if (CurrentType == ELaserType::Horizon)
	{
		// 가로 방향 설정 로직
	}
	else if (CurrentType == ELaserType::Vertical)
	{
		// 세로 방향 설정 로직 (예: Pitch 90도 회전)
		SetActorRotation(FRotator(90.f, 0.f, 0.f));
	}

	// 콜리전 및 이펙트 활성화
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	LaserEffectComp->Activate(true);
	LaserEffectComp->ReinitializeSystem();
	LaserEffectComp->SetVisibility(true);
	
}

void AFZFLaserActor::DeactivateLaser()
{
	OnLaserDeactive.Broadcast(this);
	CurrentMode = ELaserMode::Inactive;
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LaserEffectComp->Deactivate();
	LaserEffectComp->SetVisibility(false);
}

ELaserMode AFZFLaserActor::GetLaserMode()
{
	return CurrentMode;
}
