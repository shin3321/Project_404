// Fill out your copyright notice in the Description page of Project Settings.


#include "Utils/FZFGameLevelTeleport.h"
#include "Character/FZFCharacterBase.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AFZFGameLevelTeleport::AFZFGameLevelTeleport()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TransferVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TransferVolume"));

	// 충돌 설정
	TransferVolume->SetCollisionProfileName(TEXT("Trigger"));
	TransferVolume->OnComponentBeginOverlap.AddDynamic(this, &AFZFGameLevelTeleport::OnOverlapBegin);
}

// Called when the game starts or when spawned
void AFZFGameLevelTeleport::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AFZFGameLevelTeleport::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AFZFGameLevelTeleport::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority()) return;

	if (Cast<AFZFCharacterBase>(OtherActor))
	{
		FString LevelPath = TEXT("/Game/Project404/Map/FZFGameLevel");

		GetWorld()->ServerTravel(LevelPath);

	}
}
