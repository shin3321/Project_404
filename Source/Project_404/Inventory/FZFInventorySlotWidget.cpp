#include "FZFInventorySlotWidget.h"
#include "Components/Image.h"
// 슬롯에 아이템 데이터를 설정하는 함수
void UFZFInventorySlotWidget::SetSlotData(UFZFItemData* InItemData)
{
    UE_LOG(LogTemp, Warning, TEXT("SetSlotData Called"));

    if (!ItemIcon || !InItemData)
    {
        return;
    }

    if (InItemData->Icon)
    {
        // 아이콘은 항상 보이게 설정
        ItemIcon->SetBrushFromTexture(InItemData->Icon);
        ItemIcon->SetVisibility(ESlateVisibility::Visible);
        ItemIcon->SetOpacity(1.0f);
    }

    if (SelectedFrame)
    {
        // 프레임도 기본적으로 항상 보이게 설정
        SelectedFrame->SetVisibility(ESlateVisibility::Visible);
        SelectedFrame->SetOpacity(0.35f);
    }
}

// 슬롯 선택 상태를 UI에 반영하는 함수
void UFZFInventorySlotWidget::SetSelected(bool bIsSelected)
{
    UE_LOG(LogTemp, Warning, TEXT("SetSelected Called: %s"), bIsSelected ? TEXT("true") : TEXT("false"));

    if (SelectedFrame)
    {
        UE_LOG(LogTemp, Warning, TEXT("SelectedFrame Valid"));

        // 선택 상태에 따라 프레임만 opacity 변경
        SelectedFrame->SetVisibility(ESlateVisibility::Visible);
        SelectedFrame->SetOpacity(bIsSelected ? 1.0f : 0.35f);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("SelectedFrame is null"));
    }
}