#include "GAS/GA/FZFGA_DropItem.h"
#include "Character/Player/FZFCharacterPlayer.h"
#include "Inventory/FZFInventoryComponent.h"

UFZFGA_DropItem::UFZFGA_DropItem()
{
    // 인스턴싱 정책: 캐릭터마다 어빌리티 인스턴스를 가짐
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
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

    // 현재 선택된 슬롯의 아이템 버리기
    InventoryComponent->DropSelectedItem();
    Player->SetArmMeshDefaultTransform();
}