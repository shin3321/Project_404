#include "FZFHeldItemComponent.h"
#include "FZFHeldItemActor.h"
#include "Item/FZFItemData.h"
#include "Character/Player/FZFCharacterPlayer.h"
#include "Item/FZFItemAnimSetData.h"

// Item의 능력치
#include "Item/Equipment/FZFRangedItemData.h"
#include "Item/Equipment/FZFMeleeItemData.h"
#include "Item/Equipment/FZFThrowableItemData.h"

#include "GameFramework/Pawn.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"

// GAS
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"

UFZFHeldItemComponent::UFZFHeldItemComponent()
{
    // Tick 필요 없으면 끔
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);
}

void UFZFHeldItemComponent::HoldItem(UFZFItemData* ItemData)
{
    // 기존에 손에 들고 있던 아이템이 있으면 먼저 제거
    ClearHeldItem();

    // 현재 아이템 데이터 저장
    CurrentItemData = ItemData;

    // 선택된 슬롯이 비어있거나 ItemData가 없으면 종료
    if (!ItemData)
    {
        return;
    }

    // ItemData에 Mesh가 없으면 손에 보여줄 수 없으므로 종료
    if (!ItemData->Mesh)
    {
        UE_LOG(LogTemp, Warning, TEXT("ItemData Mesh is null"));
        return;
    }

    // 손에 들 아이템 Actor 클래스가 설정되어 있지 않으면 생성 불가
    if (!HeldItemClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("HeldItemClass is null"));
        return;
    }


    // 이 컴포넌트를 가지고 있는 Owner를 Character로 캐스팅
    ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
    if (!OwnerCharacter)
    {
        UE_LOG(LogTemp, Warning, TEXT("OwnerCharacter is null"));
        return;
    }

    // 캐릭터가 가진 모든 SkeletalMeshComponent를 가져옴
    // 몸 메시, 손 메시가 둘 다 있을 수 있기 때문
    TArray<USkeletalMeshComponent*> MeshComponents;
    OwnerCharacter->GetComponents<USkeletalMeshComponent>(MeshComponents);

    USkeletalMeshComponent* TargetMesh = nullptr;
    for (USkeletalMeshComponent* MeshComp : MeshComponents)
    {
        if (MeshComp && MeshComp->GetName() == TEXT("CharacterArmMesh"))
        {
            if (OwnerCharacter->IsLocallyControlled())
            {
                TargetMesh = MeshComp;
                break;
            }
            else
            {
                TargetMesh = OwnerCharacter->GetMesh();
                break;
            }
        }
    }

    // 손 메시 컴포넌트를 못 찾으면 아이템을 붙일 수 없으므로 종료
    if (!TargetMesh)
    {
        UE_LOG(LogTemp, Warning, TEXT("2_Hand SkeletalMeshComponent not found"));
        return;
    }

    // 메시 안에 hand_r_Socket 소켓이 실제로 있는지 확인
    if (!TargetMesh->DoesSocketExist(TEXT("hand_r_Socket")))
    {
        UE_LOG(LogTemp, Warning, TEXT("hand_r_Socket does not exist on 2_Hand"));
        return;
    }

    // 손에 들 아이템 Actor 생성
    CurrentHeldItem = GetWorld()->SpawnActor<AFZFHeldItemActor>(HeldItemClass);
    if (!CurrentHeldItem)
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to spawn HeldItemActor"));
        return;
    }

    // ItemData에 등록된 Mesh를 손 아이템 Actor에 적용
    CurrentHeldItem->SetHeldMesh(ItemData->Mesh);

    // 손 메시의 hand_r_Socket 소켓에 아이템 Actor를 붙임
    // SnapToTargetIncludingScale을 쓰면 소켓의 Location / Rotation / Scale이 적용됨
    CurrentHeldItem->AttachToComponent(
        TargetMesh,
        FAttachmentTransformRules::SnapToTargetIncludingScale,
        TEXT("hand_r_Socket")
    );


    AFZFCharacterPlayer* PlayerCharacter = Cast<AFZFCharacterPlayer>(GetOwner());
    if (IsValid(PlayerCharacter) && IsValid(ItemData->AnimSet))
    {
        PlayerCharacter->ApplyAnimationsByItemAnimType(ItemData->AnimSet->ThirdPersonIdle, ItemData->AnimSet->FirstPersonIdle);
    }

    if (OwnerCharacter && ItemData && ItemData->ItemAbilityTag.IsValid())
    { 
        // 캐릭터로부터 ASC를 가져옴 (IAbilitySystemInterface 구현 가정)
        if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(OwnerCharacter)) 
        { 
            UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent();
            if (ASC)
            { 
                // 아이템 장착 시 태그 부여.
                ASC->AddLooseGameplayTag(ItemData->ItemAbilityTag);
                
                // 나중에 지우기 위해 현재 태그 저장 
                CurrentEquippedTag = ItemData->ItemAbilityTag;

                // 사거리 GE 처리
                if (UFZFEquipmentItemData* EquipData = Cast<UFZFEquipmentItemData>(ItemData))
                {
                    if (RangeModifierGE.Get() && RangeDataTag.IsValid())
                    {
                        // 이 효과가 "어디서, 누구에 의해" 발생했는지에 대한 부가정보(Context)를 담을 바구니 제작
                        FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();

                        // 효과를 일으킨 주체(Instingator)와 원인 제공자(EffectCauser)를 설정
                        EffectContext.AddInstigator(OwnerCharacter, OwnerCharacter);

                        // 설정된 GE 클래스와 컨텍스트를 바탕으로, 실제 적용 가능한 실행 데이터 객체(Spec)를 생성함, 1.0f는 효과의 레벨
                        FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(RangeModifierGE, 1.0f, EffectContext);
                        if (SpecHandle.IsValid())
                        {
                            // SetByCaller로 사거리 전달
                            // GE 내부에 미리 정의된 RangeDataTag 위치에 데이터 에셋에서 가져온 실제 사거리 수치를 동적으로 넣음
                            SpecHandle.Data.Get()->SetSetByCallerMagnitude(RangeDataTag, EquipData->GetRange());

                            // GE 적용 및 핸들 저장 (나중에 해제할 때, 사용)
                            // RangeEffectHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get()); 이거와 같음
                            RangeEffectHandle = ASC->BP_ApplyGameplayEffectSpecToSelf(SpecHandle);
                        }
                    }
                }
            }
        }
    }

}

void UFZFHeldItemComponent::ClearHeldItem()
{
    // GAS 관련 자원 해제
    if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(GetOwner()))
    {
        if (UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent())
        {
            // 사거리 GE 제거 : 핸들이 유효하다면 적용 중인 GE를 제거하여 사거리 복구
            if (RangeEffectHandle.IsValid())
            {
                ASC->RemoveActiveGameplayEffect(RangeEffectHandle);
                RangeEffectHandle.Invalidate(); // 핸들 초기화
            }

            // 아이템 태그 제거 : 장착 시 부여했던 LooseTag 제거
            if (CurrentEquippedTag.IsValid())
            {
                ASC->RemoveLooseGameplayTag(CurrentEquippedTag);
                CurrentEquippedTag = FGameplayTag::EmptyTag; // 태그 초기화
            }
        }
    }

    // 데이터 포인터도 초기화
    CurrentItemData = nullptr;

    // 현재 손에 든 아이템이 있으면 제거
    if (CurrentHeldItem)
    {
        CurrentHeldItem->Destroy();
        CurrentHeldItem = nullptr;
    }
}