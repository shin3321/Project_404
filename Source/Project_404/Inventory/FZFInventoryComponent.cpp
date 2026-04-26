
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
bool UFZFInventoryComponent::AddItem(const FFZFItemData& InItemData)
{
    // 인벤토리가 가득 찼으면 추가 실패
    if (InventoryItems.Num() >= MaxItemCount)
    {
        return false;
    }

    // 인벤토리 배열에 아이템 데이터 추가
    InventoryItems.Add(InItemData);

    //sine 획득한 아이템 데이터 저장
    
    // 인벤토리 위젯이 있으면 UI 갱신
    if (InventoryWidget)
    {
        InventoryWidget->RefreshInventory(InventoryItems);
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
        InventoryWidget->RefreshInventory(InventoryItems);
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