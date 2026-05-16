#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "GameplayEffectTypes.h" // ActivateHandle 사용을 위해 필요
#include "FZFHeldItemComponent.generated.h"

//struct FGameplayTag;
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

    //GA에서 현재 아이템 정보를 참조할 수 있도록 Getter 추가
    FORCEINLINE UFZFItemData* GetCurrentItemData() const { return CurrentItemData; }

    FGameplayTag GetCurrentAttackTag() { return  CurrentEquippedTag; }

private:
    // 현재 들고 있는 아이템의 원본데이터를 저장
    UPROPERTY()
    TObjectPtr<UFZFItemData> CurrentItemData;

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

    UPROPERTY(EditAnywhere, Category = "GAS")
    FGameplayTag CurrentEquippedTag;

    // 장착 시 적용된 사거리 GE를 추적하기 위한 핸들
    FActiveGameplayEffectHandle RangeEffectHandle;

    // SetByCaller에서 사용할 데이터 태그
    UPROPERTY(EditDefaultsOnly, Category = "GAS")
    FGameplayTag RangeDataTag;

    // 무기 사거리를 보정해줄 GE 클래스 (에디터에서 설정)
    UPROPERTY(EditDefaultsOnly, Category = "GAS")
    TSubclassOf<class UGameplayEffect> RangeModifierGE;
};