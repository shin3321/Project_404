// InventoryWidget.cpp

#include "FZFInventoryWidget.h"
#include "FZFInventorySlotWidget.h"
#include "Components/WrapBox.h"



// 인벤토리 UI를 현재 아이템 목록과 선택 상태로 새로고침하는 함수
void UFZFInventoryWidget::RefreshInventory(const TArray<UFZFItemData*>& Items, int32 SelectedIndex)
{
    UE_LOG(LogTemp, Warning, TEXT("RefreshInventory Count: %d"), Items.Num());
    UE_LOG(LogTemp, Warning, TEXT("SlotWidget Created"));
    // WrapBox 또는 슬롯 위젯 클래스가 없으면 종료
    if (!InventoryWrapBox || !InventorySlotWidgetClass)
    {
        return;
    }

    // 기존 슬롯 위젯 모두 제거
    InventoryWrapBox->ClearChildren();

    // 인벤토리 아이템 배열을 순회하며 슬롯 위젯 생성
    for (int32 i = 0; i < Items.Num(); ++i)
    {
        // 슬롯 위젯 생성
        UFZFInventorySlotWidget* SlotWidget = CreateWidget<UFZFInventorySlotWidget>(GetWorld(), InventorySlotWidgetClass);
        if (!SlotWidget)
        {
            continue;
        }

        // 슬롯 위젯에 아이템 데이터 설정
        UFZFItemData* Item = Items[i];
        SlotWidget->SetSlotData(Item);

        // 현재 선택된 슬롯이면 선택 상태 표시
        SlotWidget->SetSelected(i == SelectedIndex);

        // 생성한 슬롯 위젯을 WrapBox에 추가
        InventoryWrapBox->AddChildToWrapBox(SlotWidget);
    }
}