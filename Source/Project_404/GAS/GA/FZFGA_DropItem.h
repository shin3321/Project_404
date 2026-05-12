#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "FZFGA_DropItem.generated.h"

UCLASS()
class PROJECT_404_API UFZFGA_DropItem : public UGameplayAbility
{
    GENERATED_BODY()

public:
    UFZFGA_DropItem();

    // 어빌리티가 활성화될 때 실행되는 메인 함수
    virtual void ActivateAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData
    ) override;

protected:
    // 실제 아이템 버리기 로직을 처리할 내부 함수
    void DropSelectedItem();
};