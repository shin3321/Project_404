#include "FZFInventorySlotWidget.h"
#include "Components/Image.h"
// 슬롯에 아이템 데이터를 설정하는 함수
void UFZFInventorySlotWidget::SetSlotData(UFZFItemData* InItemData)
{
    if (!InItemData)
    {
        ClearSlot();
        return;
    }

    if (ItemIcon)
    {
        ItemIcon->SetBrushFromTexture(InItemData->Icon);
        ItemIcon->SetVisibility(ESlateVisibility::Visible);
    }
}

// 슬롯 선택 상태를 UI에 반영하는 함수
void UFZFInventorySlotWidget::SetSelected(bool bIsSelected)
{
    if (SelectedFrame)
    {
        SelectedFrame->SetVisibility(ESlateVisibility::Visible);
        SelectedFrame->SetOpacity(bIsSelected ? 1.0f : 0.2f);
    }
}

void UFZFInventorySlotWidget::ClearSlot()
{
    // 아이템이 없는 슬롯은 아이콘만 숨김
    if (ItemIcon)
    {
        ItemIcon->SetBrushFromTexture(nullptr);
        ItemIcon->SetVisibility(ESlateVisibility::Hidden);
    }

    // 선택 프레임은 기본적으로 숨김
    if (SelectedFrame)
    {
        SelectedFrame->SetVisibility(ESlateVisibility::Hidden);
        SelectedFrame->SetOpacity(0.0f);
    }
}