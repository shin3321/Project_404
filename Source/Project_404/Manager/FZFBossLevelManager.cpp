// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/FZFBossLevelManager.h"
#include "Character/Monster/Boss/FZFTestBoss.h"
#include "Kismet/GameplayStatics.h"
#include "Utils/Boss/FZFLaserActor.h"
#include "Engine/TriggerBox.h"
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
			AFZFLaserActor* SpawnedLaser = GetWorld()->SpawnActor<AFZFLaserActor>(LaserClass, MinWallLocation, FRotator::ZeroRotator, SpawnParams);
			if (SpawnedLaser)
			{
				SpawnedLaser->DeactivateLaser();
				SpawnedLaser->OnLaserDeactive.AddDynamic(this, &AFZFBossLevelManager::DeactivateLaser);
				LaserPool.Add(SpawnedLaser);
			}
		}
	}
	FirstPhase();
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
	LaserCount = FMath::RandRange(2, 6);

	for (AFZFLaserActor* Laser : LaserPool)
	{
		if (Laser->GetLaserMode() == ELaserMode::Inactive)
		{
			AvailableLasers.Add(Laser);
		}
	}

	Algo::RandomShuffle(AvailableLasers);
	for (int32 i = 0; i < LaserCount; ++i)
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
			FVector SpawnLocation = BossTrigger->GetActorLocation();
			FRotator SpawnRotation = BossTrigger->GetActorRotation();
			FActorSpawnParameters SpawnParams;

			ATriggerBox* SpawnedTrigger = GetWorld()->SpawnActor<ATriggerBox>(ATriggerBox::StaticClass(), SpawnLocation, SpawnRotation, SpawnParams);

			if (SpawnedTrigger)
			{
				UBoxComponent* BoxComp = Cast<UBoxComponent>(SpawnedTrigger->GetCollisionComponent());
				if (BoxComp)
				{
					// 2. 크기를 조절합니다. 
					// 원하는 X, Y, Z 크기를 넣으세요.
					BoxComp->SetBoxExtent(FVector(200.f, 200.f, 200.f));
				}
				SpawnedTrigger->OnActorBeginOverlap.AddDynamic(this, &AFZFBossLevelManager::OnBombTriggerOverlap);
				BombTriggers.Add(SpawnedTrigger);
			}
		}
	}
}

void AFZFBossLevelManager::OnBombTriggerOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (OtherActor && OtherActor != OverlappedActor)
	{
		//AFZFCharacterPlayer* PlayerCharacter  
	}
}
