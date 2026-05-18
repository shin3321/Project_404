// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/FZFBossLevelManager.h"
#include "Character/Monster/Boss/FZFTestBoss.h"
#include "Kismet/GameplayStatics.h"
#include "Utils/Boss/FZFLaserActor.h"
#include "Utils/Boss/FZFBossTrigger.h"
#include "Utils/Boss/FZFBossBombActor.h"
#include "Components/BoxComponent.h"
#include "Algo/RandomShuffle.h"
#include "Engine/TargetPoint.h"
#include "Character/Player/FZFCharacterPlayer.h"


// Sets default values
AFZFBossLevelManager::AFZFBossLevelManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TransferVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TransferVolume"));

	RootComponent = TransferVolume;

	// 맵 크기만큼 바꾸기
	TransferVolume->InitBoxExtent(FVector(100.f, 200.f, 500.f));

	// 충돌 설정
	TransferVolume->SetCollisionProfileName(TEXT("NoCollision"));
}

// Called when the game starts or when spawned
void AFZFBossLevelManager::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
	{
		return;
	}

	AFZFTestBoss* Boss = Cast<AFZFTestBoss>(UGameplayStatics::GetActorOfClass(GetWorld(), AFZFTestBoss::StaticClass()));
	if (Boss != nullptr)
	{
		Boss->OnBossPhaseChanged.AddDynamic(this, &AFZFBossLevelManager::HandlePhaseChanged);
	}

	if (LaserClass != nullptr)
	{
		for (int32 i = 0; i < LaserCount; ++i)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			AFZFLaserActor* SpawnedLaser = GetWorld()->SpawnActor<AFZFLaserActor>(
				LaserClass, MinWallLocation, FRotator::ZeroRotator, SpawnParams);
			if (SpawnedLaser)
			{
				SpawnedLaser->DeactivateLaser();
				SpawnedLaser->OnLaserDeactive.AddDynamic(this, &AFZFBossLevelManager::DeactivateLaser);
				LaserPool.Add(SpawnedLaser);
			}
		}
	}
	SecondPhase();
}

// Called every frame
void AFZFBossLevelManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AFZFBossLevelManager::HandlePhaseChanged(EBossPhase NewPhase)
{
	switch (NewPhase)
	{
	case EBossPhase::Phase1:
	{
		FirstPhase();
		break;
	}

	case EBossPhase::Phase2:
	{
		SecondPhase();
		break;
	}
	case EBossPhase::Phase3:
	{
		break;
	}
	}
}

void AFZFBossLevelManager::FirstPhase()
{
	GetWorld()->GetTimerManager().SetTimer(
		LaserTimerHandle,
		this,
		&AFZFBossLevelManager::Laser,
		5.0f,
		true
	);

	// 페이즈 끝나면 타이머 끄기 
	/*
GetWorld()->GetTimerManager().ClearTimer(PhaseTimerHandle);
	*/
}

void AFZFBossLevelManager::Laser()
{
	AvailableLasers.Empty();
	LaserCount = FMath::RandRange(2, 6);

	for (AFZFLaserActor* Laser : LaserPool)
	{
		if (Laser->GetLaserMode() == ELaserMode::Inactive)
		{
			AvailableLasers.Add(Laser);
		}
	}

	if (AvailableLasers.IsEmpty()) return;
	Algo::RandomShuffle(AvailableLasers);

	int32 MaxSpawnCount = FMath::Min(FMath::RandRange(2, 6), AvailableLasers.Num());
	for (int32 i = 0; i < MaxSpawnCount; ++i)
	{
		AFZFLaserActor* SelectedLaser = AvailableLasers[i];
		if (SelectedLaser)
		{
			ELaserMode RandomMode = FMath::RandBool() ? ELaserMode::Moving : ELaserMode::Fixed;
			ELaserType RandomType = FMath::RandBool() ? ELaserType::Vertical : ELaserType::Horizon;

			SelectedLaser->ActivateLaser(MinWallLocation, RandomMode, RandomType, MoveSpeed);
		}
	}
}

void AFZFBossLevelManager::DeactivateLaser(AFZFLaserActor* Laser)
{
	AvailableLasers.Remove(Laser);
}

void AFZFBossLevelManager::SecondPhase()
{
	CreateTrigger();
}

void AFZFBossLevelManager::CreateTrigger()
{
	TArray<AActor*> BossTriggers;

	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("BossTriggerSlot"), BossTriggers);
	if (BossTriggers.Num() > 0)
	{
		for (AActor* BossTrigger : BossTriggers)
		{
			FVector SpawnLocation = BossTrigger->GetActorLocation() + FVector(0.0f, 0.0f, 25.0f);
			FRotator SpawnRotation = BossTrigger->GetActorRotation();
			FActorSpawnParameters SpawnParams;

			AFZFBossTrigger* SpawnedTrigger = GetWorld()->SpawnActor<AFZFBossTrigger>(AFZFBossTrigger::StaticClass(), SpawnLocation, SpawnRotation, SpawnParams);

			UE_LOG(LogTemp, Warning, TEXT("보스 트리거 스폰 지역을 찾았습니다"));
			if (SpawnedTrigger)
			{
				SpawnedTrigger->CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AFZFBossLevelManager::OnBombTriggerOverlap);
				UE_LOG(LogTemp, Warning, TEXT("보스 트리거 스폰 성공: %s"), *SpawnLocation.ToString());
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("보스 트리거 스폰 실패!"));
			}
		}
	}
}

void AFZFBossLevelManager::CreateBomb(FVector SpawnLocation)
{
	if (BombBlueprintClass == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("매니저에 폭탄 블루프린트 클래스가 설정되지 않았습니다"));
		return;
	}

	FVector BombLocation = SpawnLocation + FVector(0.0f, 0.0f, 120.0f);
	FActorSpawnParameters SpawnParams;

	AFZFBossBombActor* Bomb = GetWorld()->SpawnActor<AFZFBossBombActor>(BombBlueprintClass, BombLocation, FRotator::ZeroRotator, SpawnParams);
	if (Bomb)
	{
		UE_LOG(LogTemp, Warning, TEXT("보스 폭탄 설치됨"));
	}
	// Todo 보스에게 신호 주기
}

void AFZFBossLevelManager::OnBombTriggerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<AFZFCharacterBase>(OtherActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("컴포넌트 트리거 겹침!"));
	}
}
