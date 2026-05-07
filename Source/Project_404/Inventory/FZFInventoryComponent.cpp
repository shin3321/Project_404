
#include "FZFInventoryComponent.h"
#include "FZFInventoryWidget.h"
#include "Blueprint/UserWidget.h"

// 인벤토리 컴포넌트 생성자
UFZFInventoryComponent::UFZFInventoryComponent()
{
    // 이 컴포넌트는 Tick 사용 안 함
    PrimaryComponentTick.bCanEverTick = false;
}


// 아이템 데이터를 인벤토리에 추가하는 함수
bool UFZFInventoryComponent::AddItem(UFZFItemData* InItemData)
{
    // 아이템 데이터가 없으면 추가 실패
    if (!InItemData)
    {
        return false;
    }

    // 인벤토리가 가득 찼으면 추가 실패
    if (InventoryItems.Num() >= MaxItemCount)
    {
        return false;
    }

    // 인벤토리 배열에 아이템 데이터 추가
    InventoryItems.Add(InItemData);

    // 인벤토리 위젯이 있으면 UI 갱신
    if (InventoryWidget)
    {
        InventoryWidget->RefreshInventory(InventoryItems, SelectedSlotIndex);
    }

    // 아이템 추가 성공
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
        InventoryWidget->RefreshInventory(InventoryItems, SelectedSlotIndex);
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
    if (InSlotIndex < 0 || InSlotIndex >= InventoryItems.Num())
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
        InventoryWidget->RefreshInventory(InventoryItems, SelectedSlotIndex);
    }
}

void UFZFInventoryComponent::RemoveSelectedItem(UFZFItemData* InItemData)
{
    if (InventoryItems.Num() <= 0)
        return;

    InventoryItems.Remove(InItemData);

    if (InventoryWidget)
        InventoryWidget->RefreshInventory(InventoryItems);
}
