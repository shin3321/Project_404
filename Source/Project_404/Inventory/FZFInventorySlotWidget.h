#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Item/FZFItemData.h"
#include "FZFInventorySlotWidget.generated.h"

class UImage;

UCLASS()
class PROJECT_404_API UFZFInventorySlotWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    // 슬롯 데이터 세팅 함수
    UFUNCTION(BlueprintCallable)
    void SetSlotData(UFZFItemData* InItemData);

protected:
    // 아이템 아이콘 이미지
    UPROPERTY(meta = (BindWidget))
    UImage* ItemIcon;
};