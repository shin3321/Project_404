#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FZFHeldItemComponent.generated.h"

class AFZFHeldItemActor;
class UFZFItemData;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECT_404_API UFZFHeldItemComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    // 생성자
    UFZFHeldItemComponent();

    // 선택된 아이템을 손에 들게 하는 함수
    void HoldItem(UFZFItemData* ItemData);

    // 현재 손에 든 아이템을 제거하는 함수
    void ClearHeldItem();

private:
    // 손에 생성할 HeldItem Actor 클래스
    // BP_HeldItem 같은 블루프린트 클래스를 넣을 수 있게 함
    UPROPERTY(EditAnywhere, Category = "Held Item")
    TSubclassOf<AFZFHeldItemActor> HeldItemClass;

    // 현재 손에 들고 있는 아이템 Actor
    UPROPERTY()
    TObjectPtr<AFZFHeldItemActor> CurrentHeldItem;

    // 캐릭터 손에 만든 소켓 이름
    UPROPERTY(EditAnywhere, Category = "Held Item")
    FName HandSocketName = TEXT("hand_r_Socket");
};