// InventoryWidget.cpp

#include "FZFInventoryWidget.h"
#include "FZFInventorySlotWidget.h"
#include "Components/WrapBox.h"



// 인벤토리 UI를 현재 아이템 목록으로 새로고침하는 함수
void UFZFInventoryWidget::RefreshInventory(const TArray<TObjectPtr<UFZFItemData>>& Items)
{
    // WrapBox 또는 슬롯 위젯 클래스가 없으면 종료
    if (!InventoryWrapBox || !InventorySlotWidgetClass)
    {
        return;
    }

    // 기존 슬롯 위젯 모두 제거
    InventoryWrapBox->ClearChildren();

    // 인벤토리 아이템 배열을 순회하며 슬롯 위젯 생성
    for (UFZFItemData* Item : Items)
    {
        // 슬롯 위젯 생성
        UFZFInventorySlotWidget* SlotWidget = CreateWidget<UFZFInventorySlotWidget>(GetWorld(), InventorySlotWidgetClass);
        if (!SlotWidget)
        {
            continue;
        }

        // 슬롯 위젯에 아이템 데이터 설정
        SlotWidget->SetSlotData(Item);

        // 생성한 슬롯 위젯을 WrapBox에 추가
        InventoryWrapBox->AddChildToWrapBox(SlotWidget);
    }
}