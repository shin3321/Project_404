// InventoryWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FZFItemData.h"
#include "FZFInventoryWidget.generated.h"

class UWrapBox;
class UFZFInventorySlotWidget;

// 인벤토리 전체 UI를 관리하는 위젯 클래스
UCLASS()
class PROJECT_404_API UFZFInventoryWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    // 현재 인벤토리 아이템 목록으로 UI를 새로고침하는 함수
    UFUNCTION(BlueprintCallable)
    void RefreshInventory(const TArray<FFZFItemData>& Items);

protected:
    // 생성된 슬롯 위젯들을 담는 WrapBox
    UPROPERTY(meta = (BindWidget))
    UWrapBox* InventoryWrapBox;

    // 생성할 인벤토리 슬롯 위젯 블루프린트 클래스
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    TSubclassOf<UFZFInventorySlotWidget> InventorySlotWidgetClass;
};