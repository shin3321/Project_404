#include "GAS/GA/FZFGA_DropItem.h"
#include "Character/Player/FZFCharacterPlayer.h"
#include "Inventory/FZFInventoryComponent.h"
#include "Inventory/FZFHeldItemComponent.h"
#include "Item/FZFItemData.h"
#include "Item/Equipment/FZFEquipmentItemData.h"

UFZFGA_DropItem::UFZFGA_DropItem()
{
    // 인스턴싱 정책: 캐릭터마다 어빌리티 인스턴스를 가짐
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    
    // 네트워킹 정책: 서버에서만 실행되도록 설정하여 중복 실행(클라 RPC + 서버 예측 실행) 방지
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
}

void UFZFGA_DropItem::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData
)
{
    // 어빌리티 실행 가능 여부 확인
    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }

    // 선택된 아이템 버리기 실행
    DropSelectedItem();

    // 실행 후 즉시 어빌리티 종료
    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UFZFGA_DropItem::DropSelectedItem()
{
    // 어빌리티를 실행한 캐릭터 가져오기
    AFZFCharacterPlayer* Player = Cast<AFZFCharacterPlayer>(GetAvatarActorFromActorInfo());
    if (!Player)
    {
        return;
    }

    // 플레이어가 가진 InventoryComponent 찾기
    UFZFInventoryComponent* InventoryComponent =
        Player->FindComponentByClass<UFZFInventoryComponent>();

    // 인벤토리 컴포넌트가 없으면 종료
    if (!InventoryComponent)
    {
        return;
    }

    UFZFItemData* HeldItemData = Player->GetHeldItemComponent()->GetCurrentItemData();
    if (!IsValid(HeldItemData))
        return;

    // 도끼의 경우 버릴 수 없도록 예외처리.
    if (UFZFEquipmentItemData* EquipmentItemData = Cast<UFZFEquipmentItemData>(HeldItemData))
    {
        if (IsValid(EquipmentItemData) && EquipmentItemData->GetItemType() == EEquipmentType::Pickaxe)
            return;
    }

    // 현재 선택된 슬롯의 아이템 버리기
    InventoryComponent->DropSelectedItem();
    Player->SetArmMeshDefaultTransform();
}