// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/FZFSpawnManager.h"
#include "Character/Monster/FZFMonster.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/TargetPoint.h"

#include "Item/FZFItemBase.h"
#include "Item/FZFItemRow.h"

#include "Game/FZFGameInstance.h"

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

	UFZFGameInstance* GameInstance = Cast<UFZFGameInstance>(GetWorld()->GetGameInstance());
	if (!GameInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("GameInstance is NULL"));
	}

	ItemTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Project404/Item/DT_ItemTable"));
	if (ItemTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemTable이 생성되었습니다"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemTable이 생성되지 않았습니다"));
	}

	TArray<AActor*> SpawnPoints;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATargetPoint::StaticClass(), SpawnPoints);

	if (SpawnPoints.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("스폰 가능한 지역이 없습니다"));
		return;
	}

	// 1. 슬롯 분류
	TArray<AActor*> ItemSlots;
	TArray<AActor*> MonsterSlots;
	TArray<AActor*> StorageSlots;

	for (AActor* Point : SpawnPoints)
	{
		if (Point->ActorHasTag("ItemSpawnSlot")) ItemSlots.Add(Point);
		else if (Point->ActorHasTag("MonsterSpawnSlot")) MonsterSlots.Add(Point);
		else if (Point->ActorHasTag("StorageItemSlot")) StorageSlots.Add(Point);
	}

	// 2. 아이템 스폰 풀 생성 및 셔플
	TArray<TSubclassOf<AFZFItemBase>> ItemPool;
	for (const FFZFItemSpawnConfig& Config : ItemSpawnConfigs)
	{
		if (Config.ItemClass)
		{
			for (int32 i = 0; i < Config.SpawnCount; ++i)
			{
				ItemPool.Add(Config.ItemClass);
			}
		}
	}
	for (int32 i = ItemPool.Num() - 1; i > 0; --i)
	{
		int32 j = FMath::RandRange(0, i);
		ItemPool.Swap(i, j);
	}

	// 3. 몬스터 스폰 풀 생성 및 셔플
	TArray<TSubclassOf<AFZFMonster>> MonsterPool;
	for (const FFZFMonsterSpawnConfig& Config : MonsterSpawnConfigs)
	{
		if (Config.MonsterClass)
		{
			for (int32 i = 0; i < Config.SpawnCount; ++i)
			{
				MonsterPool.Add(Config.MonsterClass);
			}
		}
	}
	for (int32 i = MonsterPool.Num() - 1; i > 0; --i)
	{
		int32 j = FMath::RandRange(0, i);
		MonsterPool.Swap(i, j);
	}

	// 4. 아이템 스폰 실행
	if (ItemSlots.Num() != 0 && ItemPool.Num() != 0)
	{
		int32 MaxItemCount = FMath::Min(ItemSlots.Num(), ItemPool.Num());
		for (int32 i = 0; i < MaxItemCount; ++i)
		{
			AActor* Slot = ItemSlots[i];
			FVector SpawnLocation = Slot->GetActorLocation();
			FRotator SpawnRotation = Slot->GetActorRotation();
			FActorSpawnParameters SpawnParams;

			AFZFItemBase* ItemActor = GetWorld()->SpawnActor<AFZFItemBase>(ItemPool[i], SpawnLocation, SpawnRotation, SpawnParams);
			if (ItemActor)
			{
				ItemActor->ApplyGroundRotation();
				ItemActor->PlaceOnGround();
			}
		}
	}


	// 5. 몬스터 스폰 실행
	if (MonsterPool.Num() != 0)
	{
		for (int32 i = 0; i < MonsterPool.Num(); ++i)
		{
			AActor* Slot = MonsterSlots[i];
			FVector SpawnLocation = Slot->GetActorLocation();
			FRotator SpawnRotation = Slot->GetActorRotation();
			FActorSpawnParameters SpawnParams;

			GetWorld()->SpawnActor<AFZFMonster>(MonsterPool[i], SpawnLocation, SpawnRotation, SpawnParams);
		}
	}

	// 6. 창고 아이템 스폰 실행 (기존 로직 유지)
	if (GameInstance)
	{
		int32 StorageItemIdx = 0;
		int32 MaxStorageCount = FMath::Min(StorageSlots.Num(), GameInstance->StorageItems.Num());
		for (int32 i = 0; i < MaxStorageCount; ++i)
		{
			AActor* Slot = StorageSlots[i];
			FVector SpawnLocation = Slot->GetActorLocation();
			FRotator SpawnRotation = Slot->GetActorRotation();
			FActorSpawnParameters SpawnParams;

			FName ItemKey = GameInstance->StorageItems[StorageItemIdx];
			FFZFItemRow* Row = ItemTable->FindRow<FFZFItemRow>(ItemKey, TEXT("AFZFSpawnManager::SpawnStorageItem"));

			if (Row && Row->ItemActorClass)
			{
				AActor* SpawnedItem = GetWorld()->SpawnActor<AActor>(Row->ItemActorClass, SpawnLocation, SpawnRotation, SpawnParams);
				if (AFZFItemBase* ItemBase = Cast<AFZFItemBase>(SpawnedItem))
				{
					ItemBase->InitializeItem(Row->ItemData);
				}
				StorageItemIdx++;
			}
		}

		if (StorageItemIdx > 0)
		{
			GameInstance->StorageItems.Empty();
			UE_LOG(LogTemp, Warning, TEXT("서버: 창고 아이템 스폰 완료. 목록을 비웠습니다."));
		}
	}
}

// Called every frame
void AFZFSpawnManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AFZFSpawnManager::ServerSpawnItem_Implementation(FName ItemId, FVector SpawnLocation, FRotator SpawnRotation)
{
	if (!HasAuthority()) return;
	// IItemInterface::Execute_InitItem(SpawnedItem, ItemData);
	if (!ItemTable) return;

	FFZFItemRow* Row = ItemTable->FindRow<FFZFItemRow>(ItemId, TEXT("AMyCharacter::Server_SpawnItem"));
	if (!Row || !Row->ItemActorClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("해당 키에 매핑된 아이템 클래스가 없습니다: %s"), *ItemId.ToString());
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AActor* SpawnedItem = GetWorld()->SpawnActor<AActor>(Row->ItemActorClass, SpawnLocation, SpawnRotation, SpawnParams);
	if (AFZFItemBase* ItemBase = Cast<AFZFItemBase>(SpawnedItem))
	{
		// 중요: 스폰된 아이템에 데이터를 넣어주어야 상호작용 및 장착이가능합니다.
		ItemBase->InitializeItem(Row->ItemData);
		ItemBase->ApplyGroundRotation();
		ItemBase->PlaceOnGround();
	}

	if (SpawnedItem)
	{
		// 스폰된 아이템 액터에 런타임 데이터 세팅이 필요하다면 여기서 수행
		// 예: SpawnedItem->InitStats(Row->ItemName);
	}
}

bool AFZFSpawnManager::ServerSpawnItem_Validate(FName ItemId, FVector SpawnLocation, FRotator SpawnRotation)
{
	return !ItemId.IsNone();
}
