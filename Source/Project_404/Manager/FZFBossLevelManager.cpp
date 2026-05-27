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
#include "ProfilingDebugging/StallDetector.h"
#include "Physics/FZFCollision.h"
#include "Engine/StaticMeshActor.h"
#include "Components/StaticMeshComponent.h"
#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"


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

	if (LaserClass != nullptr)
	{
		for (int32 i = 0; i < LaserPoolCount; ++i)
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

	if (BossIntroDummy)
	{
		BossIntroDummy->SetActorHiddenInGame(true);
		BossIntroDummy->SetActorEnableCollision(false);
	}
}

// Called every frame
void AFZFBossLevelManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AFZFBossLevelManager::StartMapPattern(FName PatternName)
{
	UE_LOG(LogTemp, Warning, TEXT("[MapPattern] StartMapPattern: %s"), *PatternName.ToString());

	if (!HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("[MapPattern] Not Authority"));
		return;
	}

	// 맞는 패턴으로 실행하기
	if (PatternName == TEXT("MapLaser"))
	{
		Laser();

		GetWorld()->GetTimerManager().SetTimer(
			LaserTimerHandle,
			this,
			&AFZFBossLevelManager::Laser,
			2.5f, // 반복 간격
			true
		);

		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("[MapPattern] Unknown PatternName"));
}

void AFZFBossLevelManager::StopMapPattern()
{
	if (!HasAuthority())
	{
		return;
	}

	GetWorld()->GetTimerManager().ClearTimer(LaserTimerHandle);

	for (AFZFLaserActor* Laser : LaserPool)
	{
		if (Laser)
		{
			Laser->DeactivateLaser();
		}
	}

	// 필요하면 트리거/폭탄도 배열로 들고 있다가 제거
}

void AFZFBossLevelManager::Laser()
{
	UE_LOG(LogTemp, Warning, TEXT("[MapPattern] Laser Start / Pool=%d"), LaserPool.Num());

	AvailableLasers.Empty();

//	LaserPoolCount = FMath::RandRange(2, 6);

	for (AFZFLaserActor* Laser : LaserPool)
	{
		if (Laser && Laser->GetLaserMode() == ELaserMode::Inactive)
		{
			AvailableLasers.Add(Laser);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[MapPattern] Available=%d"), AvailableLasers.Num());

	if (AvailableLasers.IsEmpty()) return;
	Algo::RandomShuffle(AvailableLasers);

	const int32 SpawnCount = FMath::RandRange(
		MinSpawnLaserCount,
		MaxSpawnLaserCount
	);

	const int32 MaxSpawnCount = FMath::Min(
		SpawnCount,
		AvailableLasers.Num()
	);

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


void AFZFBossLevelManager::OnBossPhaseChanged(int32 NewPhase)
{
	if (NewPhase == 2)
	{
		CreateTrigger();
	}

	if (NewPhase == 3)
	{
		// 다른 환경 장치 생성
	}
}

/* 페이즈가 바뀜에 따라 유의미한 함정 소환 */
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

			AFZFBossTrigger* SpawnedTrigger = GetWorld()->SpawnActor<AFZFBossTrigger>(AFZFBossTrigger::StaticClass(), SpawnLocation, SpawnRotation, SpawnParams);

			UE_LOG(LogTemp, Warning, TEXT("보스 트리거 스폰 지역을 찾았습니다"));
			if (SpawnedTrigger)
			{
				SpawnedTrigger->CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

				// BossTrigger 채널로 변경
				SpawnedTrigger->CollisionBox->SetCollisionObjectType(CCHANNEL_FZFBOSSTRIGGER);

				// 일단 전부 무시
				SpawnedTrigger->CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);

				// 플레이어만 오버랩
				SpawnedTrigger->CollisionBox->SetCollisionResponseToChannel(CCHANNEL_FZFPLAYER, ECR_Overlap);

				// 혹시 플레이어 캡슐이 Pawn이면 이것도
				SpawnedTrigger->CollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

				SpawnedTrigger->CollisionBox->SetGenerateOverlapEvents(true);

				SpawnedTrigger->CollisionBox->OnComponentBeginOverlap.AddDynamic(
					this,
					&AFZFBossLevelManager::OnBombTriggerOverlap
				);
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

	FVector BombLocation = SpawnLocation;
	FActorSpawnParameters SpawnParams;

	AFZFBossBombActor* Bomb = GetWorld()->SpawnActor<AFZFBossBombActor>(BombBlueprintClass, BombLocation, FRotator::ZeroRotator, SpawnParams);
	if (Bomb)
	{
		UE_LOG(LogTemp, Warning, TEXT("보스 폭탄 설치됨"));
	}

	// 보스에게 함정 유도 신호 주기
	OnBossBombCreated.Broadcast(SpawnLocation);
}

void AFZFBossLevelManager::StartBossIntro()
{
	if (!HasAuthority() || bIntroStarted)
	{
		return;
	}

	if (BossIntroDummy)
	{
		BossIntroDummy->SetActorHiddenInGame(false);
		BossIntroDummy->SetActorEnableCollision(false);
	}

	bIntroStarted = true;

	// 문 닫기
	CloseBossDoor();

	// 연출 재생
	PlayBossIntro();
}

void AFZFBossLevelManager::OnBombTriggerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<AFZFCharacterBase>(OtherActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("컴포넌트 트리거 겹침!"));
	}
}

void AFZFBossLevelManager::OpenBossDoor()
{
	if (!BossDoor)
	{
		return;
	}

	BossDoor->SetActorHiddenInGame(true);

	if (UStaticMeshComponent* Mesh = BossDoor->GetStaticMeshComponent())
	{
		Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AFZFBossLevelManager::CloseBossDoor()
{
	if (!BossDoor)
	{
		return;
	}

	BossDoor->SetActorHiddenInGame(false);

	if (UStaticMeshComponent* Mesh = BossDoor->GetStaticMeshComponent())
	{
		Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
}

void AFZFBossLevelManager::PlayBossIntro()
{
	// 일단 비워둬도 됨.
	// 나중에 레벨 시퀀스/보스 몽타주 재생 넣기.
	if (!IntroSequenceActor)
	{
		FinishBossIntro();
		return;
	}

	ULevelSequencePlayer* Player = IntroSequenceActor->GetSequencePlayer();
	if (!Player)
	{
		FinishBossIntro();
		return;
	}

	Player->OnFinished.AddDynamic(this, &AFZFBossLevelManager::FinishBossIntro);
	Player->Play();
}

void AFZFBossLevelManager::FinishBossIntro()
{

	if (BossIntroDummy)
	{
		BossIntroDummy->SetActorHiddenInGame(true);
		BossIntroDummy->Destroy();
	}

	OnBossIntroFinished.Broadcast();
}
