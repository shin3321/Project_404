// Fill out your copyright notice in the Description page of Project Settings.


#include "Utils/Boss/FZFBossTrigger.h"
#include "Components/BoxComponent.h"
#include "Character/Player/FZFCharacterPlayer.h"
#include "Manager/FZFBossLevelManager.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AFZFBossTrigger::AFZFBossTrigger()
{
	bReplicates = true;
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	RootComponent = CollisionBox;
	CollisionBox->SetBoxExtent(FVector(200.f, 200.f, 200.f));

	TriggerMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TriggerMesh"));

	TriggerMeshComp->SetupAttachment(RootComponent);
	TriggerMeshComp->SetCollisionProfileName(TEXT("NoCollision"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Game/Project404/Utils/FZF_BossBomb/Button.Button"));
	if (MeshAsset.Succeeded())
	{
		TriggerMeshComp->SetStaticMesh(MeshAsset.Object);
	}
}

// Called when the game starts or when spawned
void AFZFBossTrigger::BeginPlay()
{
	Super::BeginPlay();
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AFZFBossTrigger::OnComponentBeginOverlap);
}

// Called every frame
void AFZFBossTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFZFBossTrigger::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 서버에서만 로직 처리 (폭탄 생성 및 자신 파괴)
	if (!HasAuthority())
	{
		return;
	}

	if (Cast<AFZFCharacterBase>(OtherActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("컴포넌트 트리거 겹침!"));
		AFZFBossLevelManager* BossLevelManager = Cast<AFZFBossLevelManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AFZFBossLevelManager::StaticClass()));

		if (BossLevelManager)
		{
			BossLevelManager->CreateBomb(GetActorLocation());
			Destroy();
		}
	}
}

