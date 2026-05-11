// InventoryWidget.cpp

#include "FZFInventoryWidget.h"
#include "FZFInventorySlotWidget.h"
#include "Components/WrapBox.h"



void UFZFInventoryWidget::RefreshInventory(
    const TArray<UFZFItemData*>& Items,
    int32 MaxSlotCount,
    int32 SelectedSlotIndex
)
{
    if (!InventoryWrapBox || !InventorySlotWidgetClass)
    {
        return;
    }

    InventoryWrapBox->ClearChildren();

    for (int32 i = 0; i < MaxSlotCount; ++i)
    {
        UFZFInventorySlotWidget* SlotWidget =
            CreateWidget<UFZFInventorySlotWidget>(this, InventorySlotWidgetClass);

        if (!SlotWidget)
        {
            continue;
        }

        // 아이템이 있는 슬롯이면 아이콘 표시
        if (Items.IsValidIndex(i) && Items[i])
        {
            SlotWidget->SetSlotData(Items[i]);
        }
        // 아이템이 없는 슬롯이면 아이콘만 비움
        else
        {
            SlotWidget->ClearSlot();
        }

        // 선택 프레임은 아이템 유무와 상관없이 슬롯 인덱스로 처리
        SlotWidget->SetSelected(i == SelectedSlotIndex);

        InventoryWrapBox->AddChildToWrapBox(SlotWidget);
    }
}