#include "FZFInventorySlotWidget.h"
#include "Components/Image.h"

// 슬롯에 아이템 데이터를 설정하는 함수
void UFZFInventorySlotWidget::SetSlotData( UFZFItemData* InItemData)
{

    if (ItemIcon)
    {
        ItemIcon->SetBrushFromTexture(InItemData->Icon);
    }
}