#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Item/FZFItemData.h"
#include "FZFInventoryComponent.generated.h"

class UFZFInventoryWidget;

// 플레이어 인벤토리 데이터와 인벤토리 UI를 관리하는 컴포넌트
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECT_404_API UFZFInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    // 인벤토리 컴포넌트 생성자
    UFZFInventoryComponent();

    // 현재 선택된 슬롯의 아이템을 버리는 함수
    // 선택 슬롯에 아이템이 없으면 아무것도 하지 않음
    void DropSelectedItem();


public:
    // 생성할 인벤토리 위젯 블루프린트 클래스
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    TSubclassOf<UFZFInventoryWidget> InventoryWidgetClass;

    // 생성된 인벤토리 위젯 인스턴스 참조
    UPROPERTY()
    UFZFInventoryWidget* InventoryWidget;

    // 인벤토리 위젯을 화면에 표시하는 함수
    UFUNCTION(BlueprintCallable)
    void ShowInventory();

    // 인벤토리 위젯을 화면에서 숨기는 함수
    UFUNCTION(BlueprintCallable)
    void HideInventory();

    void RemoveSelectedItem();

    // 현재 인벤토리에 저장된 아이템 데이터 배열
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
    TArray<TObjectPtr<UFZFItemData>> InventoryItems;

    // 인벤토리에 저장 가능한 최대 아이템 개수
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    int32 MaxItemCount = 5;

public:
    // 아이템 데이터를 인벤토리에 추가하는 함수
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool AddItem(UFZFItemData* InItemData);

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
    int32 SelectedSlotIndex = -1;

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void SelectSlot(int32 InSlotIndex);

    
    UFUNCTION()
    UFZFItemData* GetSelectedItemData() const;
    
private:
    // 현재 선택된 슬롯의 아이템을 손에 들게 하는 함수
    // 빈 슬롯이면 손에 든 아이템을 제거함
    void UpdateHeldItemBySelectedSlot();



};