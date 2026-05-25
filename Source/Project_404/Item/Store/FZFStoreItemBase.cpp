// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Store/FZFStoreItemBase.h"
#include "Game/FZFGameInstance.h"

// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/FZFItemBase.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Character/Player/FZFCharacterPlayer.h"
#include "Inventory/FZFInventoryComponent.h"

// Sets default values
AFZFStoreItemBase::AFZFStoreItemBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;

    // 네트워크 복제 설정
    bReplicates = true;
    SetReplicates(true);
}

void AFZFStoreItemBase::BeginPlay()
{
    Super::BeginPlay();
}

void AFZFStoreItemBase::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
}

void AFZFStoreItemBase::Interact(AFZFCharacterPlayer* Interactor, UPrimitiveComponent* HitComponent)
{
    if (!HasAuthority())
    {
        return;
    }

    // 창고에 스폰할 아이템 목록 저장
    UFZFGameInstance* GameInstance = Cast<UFZFGameInstance>(GetWorld()->GetGameInstance());
    if (!GameInstance)
    {
        UE_LOG(LogTemp, Warning, TEXT("GameInstance is NULL"));
    }
    else
    {
        if (ItemData)
        {
            GameInstance->AddStorageItem(ItemData->ItemId);
            UE_LOG(LogTemp, Warning, TEXT("서버: 창고에 아이템 추가됨: %s"), *ItemData->ItemId.ToString());
            Destroy();
        }
    }    
}
