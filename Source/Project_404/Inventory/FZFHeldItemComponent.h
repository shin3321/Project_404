#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "GameplayEffectTypes.h" // ActivateHandle 사용을 위해 필요
#include "FZFHeldItemComponent.generated.h"

//struct FGameplayTag;
class AFZFHeldItemActor;
class UFZFItemData;
class AFZFCharacterPlayer;

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
    FORCEINLINE UFZFItemData* GetCurrentItemData() const { return CurrentHeldItemData; }

    FGameplayTag GetCurrentAttackTag() const;

    FORCEINLINE AFZFHeldItemActor* GetHeldItemActor() const { return CurrentHeldItem; }

protected:
    virtual void BeginPlay() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    void ClearHeldItemGAS();
    void ApplyHeldItemGAS(UFZFItemData* ItemData);

// 멀티 전용 함수
/*

클라이언트 코드에서 ServerHoldItem(ItemData) 호출
        ↓
언리얼 네트워크 시스템이 RPC 요청 패킷 생성
        ↓
    서버로 전송
        ↓
서버가 해당 Actor/Component를 찾아서 RPC 실행
        ↓
ServerHoldItem_Implementation(ItemData) 호출

*/

protected:
    UFUNCTION(Server, Reliable)
    void ServerHoldItem(UFZFItemData* ItemData);

    UFUNCTION(Server, Reliable)
    void ServerClearHeldItem();


private:
    // 손에 생성할 HeldItem Actor 클래스
    // BP_HeldItem 같은 블루프린트 클래스를 넣을 수 있게 함
    UPROPERTY(EditAnywhere, Category = "Held Item")
    TSubclassOf<AFZFHeldItemActor> HeldItemClass;

    // 현재 손에 들고 있는 아이템 Actor
    UPROPERTY(ReplicatedUsing = OnRep_CurrentHeldItem)
    TObjectPtr<AFZFHeldItemActor> CurrentHeldItem;

    // 현재 들고 있는 ItemData 상태
    UPROPERTY(ReplicatedUsing = OnRep_CurrentHeldItemData)
    TObjectPtr<UFZFItemData> CurrentHeldItemData;

    // owning client에서만 쓰는 1인칭 로컬 아이템
    UPROPERTY()
    TObjectPtr<AFZFHeldItemActor> LocalFirstPersonHeldItem;

protected:
    // 복제된 3인칭 아이템 Actor를 캐릭터 Mesh에 Attach한다.
    UFUNCTION()
    void OnRep_CurrentHeldItem();

    // 복제된 아이템 데이터 기준으로 애니메이션/1인칭 아이템 표시를 갱신한다.
    UFUNCTION()
    void OnRep_CurrentHeldItemData();

    // 서버에서 다른 플레이어들에게 보일 3인칭 아이템 Actor를 생성한다.
    void SpawnThirdPersonHeldItem_Server(UFZFItemData* ItemData);

    // 서버에서 3인칭 아이템 Actor를 제거한다.
    void DestroyThirdPersonHeldItem_Server();

    // 로컬 플레이어 전용 1인칭 아이템 Actor를 생성/갱신한다.
    void RefreshLocalFirstPersonHeldItem();

    // 로컬 플레이어 전용 1인칭 아이템 Actor를 제거한다.
    void DestroyLocalFirstPersonHeldItem();

    // 3인칭 아이템 Actor를 캐릭터의 3인칭 Mesh 소켓에 부착한다.
    void AttachThirdPersonHeldItem();

    // 장착 아이템 유무에 따라 상체 블렌드 Weight를 갱신한다.
    void UpdateUpperBodyBlendWeight();

    // 장착 아이템의 AnimSet에 따라 1인칭/3인칭 Idle 애니메이션을 갱신한다.
    void UpdateHeldItemAnimation();

    AFZFCharacterPlayer* GetOwnerCharacter() const;

    UPROPERTY(EditAnywhere, Category = "GAS")
    FGameplayTag CurrentEquippedTag;

    // 장착 시 적용된 무기스탯 GE를 추적하기 위한 핸들
    FActiveGameplayEffectHandle WeaponStatEffectHandle;

    // SetByCaller에서 사용할 데이터 태그
    // 사거리
    UPROPERTY(EditDefaultsOnly, Category = "GAS")
    FGameplayTag RangeDataTag;

    // 공격력
    UPROPERTY(EditDefaultsOnly, Category = "GAS")
    FGameplayTag DamageDataTag;

    // 무기 스텟을 추가해줄 GE 클래스 (에디터에서 설정)
    UPROPERTY(EditDefaultsOnly, Category = "GAS")
    TSubclassOf<class UGameplayEffect> WeaponModifierGE;
};