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
	if (CurrentMode != ELaserMode::Moving)
		return;
	FVector NewLocation = GetActorLocation();

	if (CurrentType == ELaserType::Vertical)
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
	
	if (CurrentType == ELaserType::Horizon)
	{
		// 가로 방향 설정 로직
		SetActorRotation(FRotator(0.f, 0.f, 0.f));
	}
	else if (CurrentType == ELaserType::Vertical)
	{
		// 세로 방향 설정 로직 (예: Pitch 90도 회전)
		SetActorRotation(FRotator(90.f, 0.f, 0.f));
	}

	// 콜리전 및 이펙트 활성화
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	LaserEffectComp->Activate();
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
