// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/FZFSpawnManager.h"

#include "ContentBrowserDataFilter.h"
#include  "Character/Monster/FZFMonster.h"
#include  "Item/FZFItemBase.h"
#include "Engine/TargetPoint.h"
#include "Kismet/GameplayStatics.h"
#include "Item/FZFItemRow.h"

// Sets default values
AFZFSpawnManager::AFZFSpawnManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AFZFSpawnManager::BeginPlay()
{
	Super::BeginPlay();

	// 서버에서만 돌게 예외처리
	if (!HasAuthority())
	{
		return;
	}

	TArray<AActor*> SpawnPoints;

	// 시작 위치 설정
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATargetPoint::StaticClass(), SpawnPoints);
	if (SpawnPoints.Num() > 0)
	{
		for (AActor* SpawnPoint : SpawnPoints)
		{
			FVector SpawnLocation = SpawnPoint->GetActorLocation();
			FRotator SpawnRotation = SpawnPoint->GetActorRotation();
			FActorSpawnParameters SpawnParams;

			if (SpawnPoint->ActorHasTag("ItemSpawnSlot"))
			{
				UE_LOG(LogTemp, Warning, TEXT("아이템 스폰 지역을 찾았습니다"));

				if (ItemClasses.IsEmpty())
				{
					UE_LOG(LogTemp, Error, TEXT("ItemClasses 배열이 비어있습니다."));
					continue;
				}

				int32 RandomIndex = FMath::RandRange(0, ItemClasses.Num() - 1);
				TSubclassOf<AFZFItemBase> ItemClass = ItemClasses[RandomIndex];

				if (ItemClass)
				{
					UE_LOG(LogTemp, Warning, TEXT("ItemClass: 스폰 위치: (%lf, %f, %lf)"), SpawnLocation.X, SpawnLocation.Y,
					       SpawnLocation.Z);
					GetWorld()->SpawnActor<AFZFItemBase>(ItemClass, SpawnLocation, SpawnRotation, SpawnParams);
				}
			}
			else if (SpawnPoint->ActorHasTag("MonsterSpawnSlot"))
			{
				UE_LOG(LogTemp, Warning, TEXT("몬스터 스폰 지역을 찾았습니다"));

				if (MonsterClasses.IsEmpty())
				{
					UE_LOG(LogTemp, Error, TEXT("MonsterClasses 배열이 비어있습니다."));
					continue;
				}

				int32 RandomIndex = FMath::RandRange(0, MonsterClasses.Num() - 1);
				TSubclassOf<AFZFMonster> MonsterClass = MonsterClasses[RandomIndex];

				if (MonsterClass)
				{
					UE_LOG(LogTemp, Warning, TEXT("MonsterClass: 스폰 위치: (%lf, %f, %lf)"), SpawnLocation.X,
					       SpawnLocation.Y, SpawnLocation.Z);
					GetWorld()->SpawnActor<AFZFMonster>(MonsterClass, SpawnLocation, SpawnRotation, SpawnParams);
				}
			}
		}
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("스폰 가능한 지역이 없습니다"));
}

// Called every frame
void AFZFSpawnManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AFZFSpawnManager::SpawnItem_Implementation(FName ItemId, FVector SpawnLocation)
{
	if (!HasAuthority()) return;
// IItemInterface::Execute_InitItem(SpawnedItem, ItemData);
	UDataTable* ItemTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(),nullptr, TEXT("/Game/Project404/Item/DT_FZFItemTable.DT_FZFItemTable")));
	if (!ItemTable) return;
	
	FFZFItemRow* Row = ItemTable->FindRow<FFZFItemRow>(ItemId, TEXT("AMyCharacter::Server_SpawnItem"));
	if (!Row || !Row->ItemActorClass) 
	{
		UE_LOG(LogTemp, Warning, TEXT("해당 키에 매핑된 아이템 클래스가 없습니다: %s"), *ItemId.ToString());
		return;
	}
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	FRotator SpawnRotation = FRotator::ZeroRotator;

	AActor* SpawnedItem = GetWorld()->SpawnActor<AActor>(Row->ItemActorClass, SpawnLocation, SpawnRotation, SpawnParams);

	if (SpawnedItem)
	{
		// 스폰된 아이템 액터에 런타임 데이터 세팅이 필요하다면 여기서 수행
		// 예: SpawnedItem->InitStats(Row->ItemName);
	}
}

bool AFZFSpawnManager::SpawnItem_Validate(FName ItemId, FVector SpawnLocation)
{
	return !ItemId.IsNone();
}
