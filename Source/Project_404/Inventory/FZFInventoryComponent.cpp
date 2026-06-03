
#include "FZFInventoryComponent.h"
#include "FZFInventoryWidget.h"
#include "Blueprint/UserWidget.h"
#include "FZFHeldItemComponent.h"
#include "Character/Player/FZFCharacterPlayer.h"
#include "Character/Player/FZFPlayerController.h"
#include "Item/FZFItemData.h"
#include "Item/FZFItemBase.h"
#include "Kismet/GameplayStatics.h"
#include "Manager/FZFSpawnManager.h"
#include "Character/Player/FZFPlayerState.h"

#include "Net/UnrealNetwork.h"

// 인벤토리 컴포넌트 생성자
UFZFInventoryComponent::UFZFInventoryComponent()
{
    // 이 컴포넌트는 Tick 사용 안 함
    PrimaryComponentTick.bCanEverTick = false;

    SetIsReplicatedByDefault(true);
    InventoryItems.SetNum(MaxItemCount);
}

void UFZFInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void UFZFInventoryComponent::SyncFromPlayerState()
{
    UE_LOG(LogTemp, Warning, TEXT("=== InventoryComponent::SyncFromPlayerState ==="));
    UE_LOG(LogTemp, Warning, TEXT("Owner: %s Authority=%d"),
        *GetNameSafe(GetOwner()),
        GetOwner() ? GetOwner()->HasAuthority() : false);

    AFZFCharacterPlayer* OwnerPlayer = Cast<AFZFCharacterPlayer>(GetOwner());
    if (!OwnerPlayer)
    {
        UE_LOG(LogTemp, Warning, TEXT("Sync Failed: OwnerPlayer null"));
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("Owner IsLocallyControlled=%d"), OwnerPlayer->IsLocallyControlled());

    AFZFPlayerState* PS = OwnerPlayer->GetPlayerState<AFZFPlayerState>();
    if (!PS)
    {
        UE_LOG(LogTemp, Warning, TEXT("Sync Failed: PlayerState null"));
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("PS: %s Authority=%d"), *GetNameSafe(PS), PS->HasAuthority());
    UE_LOG(LogTemp, Warning, TEXT("PS InventoryItemIds Num=%d"), PS->InventoryItemIds.Num());

    MaxItemCount = PS->MaxItemCount;
    SelectedSlotIndex = PS->SelectedSlotIndex;

    InventoryItems.Empty();
    InventoryItems.SetNum(MaxItemCount);

    for (int32 i = 0; i < PS->InventoryItemIds.Num(); ++i)
    {
        if (!InventoryItems.IsValidIndex(i))
        {
            continue;
        }

        const FName ItemId = PS->InventoryItemIds[i];

        UE_LOG(LogTemp, Warning, TEXT("Sync Slot[%d] ItemId=%s"),
            i,
            *ItemId.ToString());

        if (ItemId.IsNone())
        {
            InventoryItems[i] = nullptr;
        }
        else
        {
            UFZFItemData* FoundData = FindItemDataById(ItemId);

            UE_LOG(LogTemp, Warning, TEXT("FindItemDataById Result Slot[%d]: %s"),
                i,
                *GetNameSafe(FoundData));

            InventoryItems[i] = FoundData;
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("InventoryWidget: %s"), *GetNameSafe(InventoryWidget));

    if (InventoryWidget)
    {
        InventoryWidget->RefreshInventory(InventoryItems, MaxItemCount, SelectedSlotIndex);
    }

    UpdateHeldItemBySelectedSlot();
}
void UFZFInventoryComponent::OnRep_InventoryItems()
{
    if (InventoryWidget)
    {
        InventoryWidget->RefreshInventory(InventoryItems, MaxItemCount, SelectedSlotIndex);
    }
}

void UFZFInventoryComponent::OnRep_SelectedSlotIndex()
{
    // 선택 상태 변경 후 UI 갱신
    if (InventoryWidget)
    {
        InventoryWidget->RefreshInventory(InventoryItems, MaxItemCount, SelectedSlotIndex);
    }

    // 선택된 슬롯에 맞춰 손에 들 아이템 갱신
    UpdateHeldItemBySelectedSlot();
}

void UFZFInventoryComponent::InitializeComponent()
{
    Super::InitializeComponent();
    SpawnManager = Cast<AFZFSpawnManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AFZFSpawnManager::StaticClass()));
}

bool UFZFInventoryComponent::AddItem(UFZFItemData* InItemData)
{
    UE_LOG(LogTemp, Warning, TEXT("AddItem Called. Authority=%d Item=%s"),
        GetOwner() ? GetOwner()->HasAuthority() : false,
        InItemData ? *InItemData->ItemId.ToString() : TEXT("NULL"));

    if (!InItemData)
    {
        return false;
    }

    AActor* OwnerActor = GetOwner();
    if (!OwnerActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("AddItem Failed: OwnerActor null"));
        return false;
    }

    if (!OwnerActor->HasAuthority())
    {
        UE_LOG(LogTemp, Warning, TEXT("AddItem Client -> ServerAddItemById"));
        ServerAddItemById(InItemData->ItemId);
        return true;
    }

    AFZFCharacterPlayer* OwnerPlayer = Cast<AFZFCharacterPlayer>(OwnerActor);
    if (!OwnerPlayer)
    {
        UE_LOG(LogTemp, Warning, TEXT("AddItem Failed: Owner is not AFZFCharacterPlayer"));
        return false;
    }

    AFZFPlayerState* PS = OwnerPlayer->GetPlayerState<AFZFPlayerState>();
    if (!PS)
    {
        UE_LOG(LogTemp, Warning, TEXT("AddItem Failed: PlayerState null"));
        return false;
    }

    const bool bAdded = PS->AddItemId(InItemData->ItemId);

    UE_LOG(LogTemp, Warning, TEXT("AddItem Result: %d"), bAdded);

    if (bAdded)
    {
        SyncFromPlayerState();

        AFZFPlayerController* PC = Cast<AFZFPlayerController>(OwnerPlayer->GetController());

        UE_LOG(LogTemp, Warning, TEXT("AddItem OwnerPlayer=%s Controller=%s PC=%s"),
            *GetNameSafe(OwnerPlayer),
            *GetNameSafe(OwnerPlayer->GetController()),
            *GetNameSafe(PC));

        if (PC)
        {
            UE_LOG(LogTemp, Warning, TEXT("Calling ClientApplyInventorySnapshot"));

            PC->ClientApplyInventorySnapshot(
                PS->InventoryItemIds,
                PS->SelectedSlotIndex
            );
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("ClientApplyInventorySnapshot Failed: PC is null or not AFZFPlayerController"));
        }
    }

    return bAdded;
}

void UFZFInventoryComponent::ServerAddItemById_Implementation(FName ItemId)
{
    UE_LOG(LogTemp, Warning, TEXT("ServerAddItemById Called. Authority=%d ItemId=%s"),
        GetOwner() ? GetOwner()->HasAuthority() : false,
        *ItemId.ToString());

    AFZFCharacterPlayer* OwnerPlayer = Cast<AFZFCharacterPlayer>(GetOwner());
    if (!OwnerPlayer)
    {
        UE_LOG(LogTemp, Warning, TEXT("ServerAddItemById Failed: OwnerPlayer null"));
        return;
    }

    AFZFPlayerState* PS = OwnerPlayer->GetPlayerState<AFZFPlayerState>();
    if (!PS)
    {
        UE_LOG(LogTemp, Warning, TEXT("ServerAddItemById Failed: PlayerState null"));
        return;
    }

    const bool bAdded = PS->AddItemId(ItemId);

    UE_LOG(LogTemp, Warning, TEXT("ServerAddItemById Add Result: %d"), bAdded);

    if (bAdded)
    {
        // 서버 쪽 상태 갱신
        SyncFromPlayerState();

        AFZFPlayerController* PC = Cast<AFZFPlayerController>(OwnerPlayer->GetController());

        UE_LOG(LogTemp, Warning, TEXT("ServerAddItemById OwnerPlayer=%s Controller=%s PC=%s"),
            *GetNameSafe(OwnerPlayer),
            *GetNameSafe(OwnerPlayer->GetController()),
            *GetNameSafe(PC));

        if (PC)
        {
            UE_LOG(LogTemp, Warning, TEXT("Calling ClientApplyInventorySnapshot"));

            PC->ClientApplyInventorySnapshot(
                PS->InventoryItemIds,
                PS->SelectedSlotIndex
            );
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("ClientApplyInventorySnapshot Failed: PC null"));
        }
    }
}

// 인벤토리 위젯을 화면에 표시하는 함수
void UFZFInventoryComponent::ShowInventory()
{
    UE_LOG(LogTemp, Warning, TEXT("=== ShowInventory ==="));

    AActor* OwnerActor = GetOwner();
    APawn* OwnerPawn = Cast<APawn>(OwnerActor);

    UE_LOG(LogTemp, Warning, TEXT("Owner: %s Authority=%d"),
        *GetNameSafe(OwnerActor),
        OwnerActor ? OwnerActor->HasAuthority() : false);

    UE_LOG(LogTemp, Warning, TEXT("OwnerPawn: %s Local=%d"),
        *GetNameSafe(OwnerPawn),
        OwnerPawn ? OwnerPawn->IsLocallyControlled() : false);

    if (AFZFCharacterPlayer* OwnerPlayer = Cast<AFZFCharacterPlayer>(OwnerActor))
    {
        AFZFPlayerState* PS = OwnerPlayer->GetPlayerState<AFZFPlayerState>();

        UE_LOG(LogTemp, Warning, TEXT("ShowInventory PS: %s Authority=%d"),
            *GetNameSafe(PS),
            PS ? PS->HasAuthority() : false);

        if (PS)
        {
            for (int32 i = 0; i < PS->InventoryItemIds.Num(); ++i)
            {
                UE_LOG(LogTemp, Warning, TEXT("ShowInventory PS Slot[%d] = %s"),
                    i,
                    *PS->InventoryItemIds[i].ToString());
            }
        }
    }

    if (!OwnerPawn || !OwnerPawn->IsLocallyControlled())
    {
        UE_LOG(LogTemp, Warning, TEXT("ShowInventory Failed: not local pawn"));
        return;
    }

    if (!InventoryWidgetClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("ShowInventory Failed: InventoryWidgetClass null"));
        return;
    }

    if (!InventoryWidget)
    {
        InventoryWidget = CreateWidget<UFZFInventoryWidget>(GetWorld(), InventoryWidgetClass);
        UE_LOG(LogTemp, Warning, TEXT("Created InventoryWidget: %s"), *GetNameSafe(InventoryWidget));
    }

    if (InventoryWidget)
    {
        SyncFromPlayerState();

        InventoryWidget->AddToViewport();
        //InventoryWidget->RefreshInventory(InventoryItems, MaxItemCount, SelectedSlotIndex);
    }
}
// 인벤토리 위젯을 화면에서 제거하는 함수
void UFZFInventoryComponent::HideInventory()
{
    // 생성된 인벤토리 위젯이 있으면 화면에서 제거
    if (InventoryWidget)
    {
        InventoryWidget->RemoveFromParent();
    }
}

// 선택한 슬롯 인덱스를 저장하는 함수
void UFZFInventoryComponent::SelectSlot(int32 InSlotIndex)
{
    AActor* OwnerActor = GetOwner();
    if (!OwnerActor)
    {
        return;
    }

    if (!OwnerActor->HasAuthority())
    {
        ServerSelectSlot(InSlotIndex);

        // 로컬 즉시 반응용
        SelectedSlotIndex = InSlotIndex;
        if (InventoryWidget)
        {
            InventoryWidget->RefreshInventory(InventoryItems, MaxItemCount, SelectedSlotIndex);
        }

        UpdateHeldItemBySelectedSlot();
        return;
    }

    AFZFCharacterPlayer* OwnerPlayer = Cast<AFZFCharacterPlayer>(OwnerActor);
    if (!OwnerPlayer)
    {
        return;
    }

    AFZFPlayerState* PS = OwnerPlayer->GetPlayerState<AFZFPlayerState>();
    if (!PS)
    {
        return;
    }

    PS->SetSelectedSlotIndex(InSlotIndex);

    SyncFromPlayerState();
}


void UFZFInventoryComponent::ServerSelectSlot_Implementation(int32 InSlotIndex)
{
    SelectSlot(InSlotIndex);
}

UFZFItemData* UFZFInventoryComponent::GetSelectedItemData() const
{
    if (SelectedSlotIndex < 0 || SelectedSlotIndex > InventoryItems.Num() - 1)
        return nullptr;

    return InventoryItems[SelectedSlotIndex];
}

void UFZFInventoryComponent::RemoveSelectedItem()
{
    AActor* OwnerActor = GetOwner();
    if (!OwnerActor)
    {
        return;
    }

    if (!OwnerActor->HasAuthority())
    {
        ServerRemoveSelectedItem();
        return;
    }

    AFZFCharacterPlayer* OwnerPlayer = Cast<AFZFCharacterPlayer>(OwnerActor);
    if (!OwnerPlayer)
    {
        return;
    }

    AFZFPlayerState* PS = OwnerPlayer->GetPlayerState<AFZFPlayerState>();
    if (!PS)
    {
        return;
    }

    const int32 SlotIndex = PS->SelectedSlotIndex;

    if (SlotIndex < 0 || SlotIndex >= PS->MaxItemCount)
    {
        return;
    }

    PS->RemoveItemAt(SlotIndex);

    SyncFromPlayerState();
}

void UFZFInventoryComponent::ServerRemoveSelectedItem_Implementation()
{
    RemoveSelectedItem();
}

void UFZFInventoryComponent::UpdateHeldItemBySelectedSlot()
{
    // 이 컴포넌트를 가진 Owner에서 HeldItemComponent 찾기
    UFZFHeldItemComponent* HeldItemComponent =
        GetOwner()->FindComponentByClass<UFZFHeldItemComponent>();

    // 캐릭터에 HeldItemComponent가 없으면 처리 불가
    if (!HeldItemComponent)
    {
        return;
    }

    // 선택 인덱스가 InventoryItems 배열 안에 없으면 빈 슬롯으로 판단
    // 예: 슬롯은 5개인데 아이템은 2개만 있을 수 있음
    if (!InventoryItems.IsValidIndex(SelectedSlotIndex))
    {
        HeldItemComponent->ClearHeldItem();
        return;
    }

    // 선택된 슬롯의 ItemData 가져오기
    UFZFItemData* SelectedItemData = InventoryItems[SelectedSlotIndex];

    // 선택 슬롯에 아이템 데이터가 없으면 손 아이템 제거
    if (!SelectedItemData)
    {
        HeldItemComponent->ClearHeldItem();
        return;
    }

    // 선택된 아이템을 손에 들게 함
    HeldItemComponent->HoldItem(SelectedItemData);
}

UFZFItemData* UFZFInventoryComponent::FindItemDataById(FName ItemId) const
{
    UE_LOG(LogTemp, Warning, TEXT("=== FindItemDataById ==="));
    UE_LOG(LogTemp, Warning, TEXT("World: %s"), GetWorld() ? *GetWorld()->GetName() : TEXT("NULL"));
    UE_LOG(LogTemp, Warning, TEXT("ItemId: %s"), *ItemId.ToString());
    UE_LOG(LogTemp, Warning, TEXT("Cached SpawnManager: %s"), *GetNameSafe(SpawnManager));

    if (ItemId.IsNone())
    {
        return nullptr;
    }

    AFZFSpawnManager* FoundSpawnManager = SpawnManager;

    if (!FoundSpawnManager)
    {
        FoundSpawnManager = Cast<AFZFSpawnManager>(
            UGameplayStatics::GetActorOfClass(
                GetWorld(),
                AFZFSpawnManager::StaticClass()
            )
        );
    }

    UE_LOG(LogTemp, Warning, TEXT("Found SpawnManager: %s"), *GetNameSafe(FoundSpawnManager));

    if (!FoundSpawnManager)
    {
        UE_LOG(LogTemp, Warning, TEXT("FindItemDataById failed. SpawnManager is null."));
        return nullptr;
    }

    UFZFItemData* Result = FoundSpawnManager->GetItemDataById(ItemId);

    UE_LOG(LogTemp, Warning, TEXT("GetItemDataById Result: %s"), *GetNameSafe(Result));

    return Result;
}

void UFZFInventoryComponent::DropSelectedItem()
{
    if (!InventoryItems.IsValidIndex(SelectedSlotIndex))
    {
        return;
    }

    UFZFItemData* SelectedItemData = InventoryItems[SelectedSlotIndex];
    if (!SelectedItemData)
    {
        return;
    }

    AActor* OwnerActor = GetOwner();
    if (!OwnerActor)
    {
        return;
    }

    FVector DropLocation =
        OwnerActor->GetActorLocation() +
        OwnerActor->GetActorForwardVector() * 150.0f;

    DropLocation.Z -= 50.0f;

    FRotator DropRotation = OwnerActor->GetActorRotation();

    const int32 SlotIndexToClear = SelectedSlotIndex;

    if (OwnerActor->HasAuthority())
    {
        ServerDropItem_Implementation(SlotIndexToClear, DropLocation, DropRotation);
    }
    else
    {
        ServerDropItem(SlotIndexToClear, DropLocation, DropRotation);
    }
}

void UFZFInventoryComponent::ServerDropItem_Implementation(
    int32 SlotIndex,
    FVector SpawnLoc,
    FRotator SpawnRot)
{
    AFZFCharacterPlayer* OwnerPlayer = Cast<AFZFCharacterPlayer>(GetOwner());
    if (!OwnerPlayer)
    {
        return;
    }

    AFZFPlayerState* PS = OwnerPlayer->GetPlayerState<AFZFPlayerState>();
    if (!PS)
    {
        return;
    }

    if (!PS->InventoryItemIds.IsValidIndex(SlotIndex))
    {
        return;
    }

    const FName ItemId = PS->InventoryItemIds[SlotIndex];

    if (ItemId.IsNone())
    {
        return;
    }

    // 서버 인벤토리에서 제거
    PS->RemoveItemAt(SlotIndex);

    // 서버 UI / 손 아이템 갱신
    SyncFromPlayerState();

    AFZFPlayerController* PC = Cast<AFZFPlayerController>(OwnerPlayer->GetController());
    if (PC)
    {
        PC->RequestSpawnItem(ItemId, SpawnLoc, SpawnRot);
    }
}

void UFZFInventoryComponent::ApplyInventorySnapshot(
    const TArray<FName>& NewInventoryItemIds,
    int32 NewSelectedSlotIndex)
{
    UE_LOG(LogTemp, Warning, TEXT("=== ApplyInventorySnapshot ==="));

    MaxItemCount = NewInventoryItemIds.Num();
    SelectedSlotIndex = NewSelectedSlotIndex;

    InventoryItems.Empty();
    InventoryItems.SetNum(MaxItemCount);

    for (int32 i = 0; i < NewInventoryItemIds.Num(); ++i)
    {
        const FName ItemId = NewInventoryItemIds[i];

        UE_LOG(LogTemp, Warning, TEXT("Snapshot Slot[%d]=%s"),
            i,
            *ItemId.ToString());

        if (ItemId.IsNone())
        {
            InventoryItems[i] = nullptr;
        }
        else
        {
            InventoryItems[i] = FindItemDataById(ItemId);
        }
    }

    AFZFCharacterPlayer* OwnerPlayer = Cast<AFZFCharacterPlayer>(GetOwner());

    UE_LOG(LogTemp, Warning, TEXT("Snapshot Owner=%s Local=%d Widget=%s WidgetClass=%s"),
        *GetNameSafe(OwnerPlayer),
        OwnerPlayer ? OwnerPlayer->IsLocallyControlled() : false,
        *GetNameSafe(InventoryWidget),
        *GetNameSafe(InventoryWidgetClass));

    if (OwnerPlayer && OwnerPlayer->IsLocallyControlled())
    {
        if (!InventoryWidget && InventoryWidgetClass)
        {
            InventoryWidget = CreateWidget<UFZFInventoryWidget>(GetWorld(), InventoryWidgetClass);

            if (InventoryWidget)
            {
                InventoryWidget->AddToViewport();
                UE_LOG(LogTemp, Warning, TEXT("Snapshot Created InventoryWidget: %s"),
                    *GetNameSafe(InventoryWidget));
            }
        }

        if (InventoryWidget)
        {
            InventoryWidget->RefreshInventory(InventoryItems, MaxItemCount, SelectedSlotIndex);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Snapshot Refresh Failed: InventoryWidget null"));
        }
    }

    UpdateHeldItemBySelectedSlot();
}