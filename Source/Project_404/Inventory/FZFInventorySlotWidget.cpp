#include "FZFInventorySlotWidget.h"
#include "Components/Image.h"

// 슬롯에 아이템 데이터를 설정하는 함수
void UFZFInventorySlotWidget::SetSlotData(const FFZFItemData& InItemData)
{
    // 아이콘 이미지 위젯과 아이템 아이콘 텍스처가 모두 있으면 슬롯 이미지 갱신
    if (ItemIcon && InItemData.Icon)
    {
        ItemIcon->SetBrushFromTexture(InItemData.Icon);
    }
}