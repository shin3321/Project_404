
#include "FZFInventoryComponent.h"
#include "FZFInventoryWidget.h"
#include "Blueprint/UserWidget.h"
#include "FZFHeldItemComponent.h"
#include "Item/FZFItemData.h"
#include "Inventory/FZFItemDataComponent.h"

// 인벤토리 컴포넌트 생성자
UFZFInventoryComponent::UFZFInventoryComponent()
{
    // 이 컴포넌트는 Tick 사용 안 함
    PrimaryComponentTick.bCanEverTick = false;

    InventoryItems.SetNum(MaxItemCount);
}


bool UFZFInventoryComponent::AddItem(UFZFItemData* InItemData)
{
    if (!InItemData)
    {
        return false;
    }

    int32 EmptySlotIndex = -1;

    // 빈 슬롯 찾기
    for (int32 i = 0; i < MaxItemCount; ++i)
    {
        if (InventoryItems[i] == nullptr)
        {
            EmptySlotIndex = i;
            break;
        }
    }

    // 빈 슬롯 없으면 추가 실패
    if (EmptySlotIndex == -1)
    {
        return false;
    }

    // 빈 슬롯에 아이템 넣기
    InventoryItems[EmptySlotIndex] = InItemData;

    if (InventoryWidget)
    {
        InventoryWidget->RefreshInventory(InventoryItems, MaxItemCount, SelectedSlotIndex);
    }

    // 현재 선택된 슬롯에 아이템이 들어왔으면 바로 손에 들기
    if (SelectedSlotIndex == EmptySlotIndex)
    {
        UpdateHeldItemBySelectedSlot();
    }

    return true;
}

// 인벤토리 위젯을 화면에 표시하는 함수
void UFZFInventoryComponent::ShowInventory()
{
    // 생성할 인벤토리 위젯 클래스가 없으면 종료
    if (!InventoryWidgetClass)
    {
        return;
    }

    // 인벤토리 위젯이 아직 없으면 생성
    if (!InventoryWidget)
    {
        InventoryWidget = CreateWidget<UFZFInventoryWidget>(GetWorld(), InventoryWidgetClass);
    }

    // 생성된 인벤토리 위젯을 화면에 표시하고 현재 아이템 목록으로 갱신
    if (InventoryWidget)
    {
        InventoryWidget->AddToViewport();
        InventoryWidget->RefreshInventory(InventoryItems, MaxItemCount, SelectedSlotIndex);
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
    // 선택한 슬롯 번호가 인벤토리 범위를 벗어나면 선택 해제
    if (InSlotIndex < 0 || InSlotIndex >= MaxItemCount)
    {
        SelectedSlotIndex = -1;
    }
    else
    {
        // 현재 선택 슬롯 인덱스 저장
        SelectedSlotIndex = InSlotIndex;
    }

    UE_LOG(LogTemp, Warning, TEXT("Selected Slot Index: %d"), SelectedSlotIndex);

    // 선택 상태 변경 후 UI 갱신
    if (InventoryWidget)
    {
        InventoryWidget->RefreshInventory(InventoryItems, MaxItemCount, SelectedSlotIndex);
    }

    // 선택된 슬롯에 맞춰 손에 들 아이템 갱신
    UpdateHeldItemBySelectedSlot();
}

UFZFItemData* UFZFInventoryComponent::GetSelectedItemData() const
{
    if (SelectedSlotIndex < 0 || SelectedSlotIndex > InventoryItems.Num() - 1)
        return nullptr;

    return InventoryItems[SelectedSlotIndex];
}

void UFZFInventoryComponent::RemoveSelectedItem()
{
    if (SelectedSlotIndex < 0 || SelectedSlotIndex >= MaxItemCount)
        return;

    InventoryItems[SelectedSlotIndex] = nullptr;

    if (InventoryWidget)
        InventoryWidget->RefreshInventory(InventoryItems, MaxItemCount, SelectedSlotIndex);

    // 삭제 후 현재 선택 슬롯 기준으로 손 아이템 다시 갱신
    UpdateHeldItemBySelectedSlot();
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

void UFZFInventoryComponent::DropSelectedItem()
{
    // 선택된 슬롯 번호가 인벤토리 배열 범위 밖이면 버릴 아이템이 없음
    if (!InventoryItems.IsValidIndex(SelectedSlotIndex))
    {
        return;
    }

    // 선택된 슬롯의 ItemData 가져오기
    UFZFItemData* SelectedItemData = InventoryItems[SelectedSlotIndex];

    // 선택된 슬롯에 ItemData가 없으면 종료
    if (!SelectedItemData)
    {
        return;
    }

    // 버렸을 때 다시 생성할 BP 아이템 클래스가 없으면 Spawn 불가
    if (!SelectedItemData->DroppedItemActorClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("DroppedItemActorClass is null"));
        return;
    }

    // 이 InventoryComponent를 가지고 있는 Owner 가져오기
    // 보통 플레이어 캐릭터 또는 플레이어 Pawn
    AActor* OwnerActor = GetOwner();
    if (!OwnerActor)
    {
        return;
    }

    // 플레이어 위치 기준으로 아이템을 버릴 위치 계산
    // 현재 위치에서 앞쪽으로 150만큼 떨어진 곳에 생성
    FVector DropLocation =
        OwnerActor->GetActorLocation() +
        OwnerActor->GetActorForwardVector() * 150.0f;

    // 아이템이 바닥 근처에 생기도록 Z값 살짝 낮춤
    DropLocation.Z -= 50.0f;

    // 플레이어가 바라보는 방향으로 아이템 회전 설정
    FRotator DropRotation = OwnerActor->GetActorRotation();

    // 월드에 다시 BP 아이템 Actor 생성
    AActor* DroppedActor = GetWorld()->SpawnActor<AActor>(
        SelectedItemData->DroppedItemActorClass,
        DropLocation,
        DropRotation
    );

    // Spawn 실패하면 종료
    if (!DroppedActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to spawn dropped item actor"));
        return;
    }

    // 생성된 BP 아이템 Actor 안에 있는 ItemDataComponent를 찾음
    // 이 컴포넌트가 있어야 다시 주울 때 어떤 아이템인지 알 수 있음
    UFZFItemDataComponent* ItemDataComponent =
        DroppedActor->FindComponentByClass<UFZFItemDataComponent>();

    // ItemDataComponent가 있으면 방금 버린 ItemData를 다시 넣어줌
    if (ItemDataComponent)
    {
        ItemDataComponent->ItemData = SelectedItemData;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Dropped actor has no ItemDataComponent"));
    }

    // 인벤토리 배열에서 선택된 아이템 제거
    // 배열 크기는 유지하고 해당 슬롯만 비움
    InventoryItems[SelectedSlotIndex] = nullptr;

    // UI 갱신
    if (InventoryWidget)
    {
        InventoryWidget->RefreshInventory(InventoryItems, MaxItemCount, SelectedSlotIndex);
    }

    // 현재 선택 슬롯 기준으로 손에 든 아이템 갱신
    // 버린 슬롯이 비었으면 손 아이템도 제거됨
    UpdateHeldItemBySelectedSlot();
}
