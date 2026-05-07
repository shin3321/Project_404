#include "FZFInventorySlotWidget.h"
#include "Components/Image.h"

// 슬롯에 아이템 데이터를 설정하는 함수
void UFZFInventorySlotWidget::SetSlotData( UFZFItemData* InItemData)
{
    UE_LOG(LogTemp, Warning, TEXT("SetSlotData Called"));

    if (ItemIcon)
    {
        ItemIcon->SetBrushFromTexture(InItemData->Icon);
    }
}

// 슬롯 선택 상태를 UI에 반영하는 함수
void UFZFInventorySlotWidget::SetSelected(bool bIsSelected)
{
    UE_LOG(LogTemp, Warning, TEXT("SetSelected Called: %s"), bIsSelected ? TEXT("true") : TEXT("false"));

    if (ItemIcon)
    {
        // 선택 안 된 슬롯도 보이게 너무 낮추지 않기
        ItemIcon->SetOpacity(bIsSelected ? 1.0f : 0.8f);
        ItemIcon->SetVisibility(ESlateVisibility::Visible);
    }
    if (SelectedFrame)
    {
        UE_LOG(LogTemp, Warning, TEXT("SelectedFrame Valid"));
        // 선택된 슬롯만 프레임 표시
        SelectedFrame->SetVisibility(bIsSelected ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("SelectedFrame is null"));
    }
}