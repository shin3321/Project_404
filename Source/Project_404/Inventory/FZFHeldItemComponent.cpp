#include "FZFHeldItemComponent.h"
#include "FZFHeldItemActor.h"
#include "Item/FZFItemData.h"
#include "Character/Player/FZFCharacterPlayer.h"
#include "Item/FZFItemAnimSetData.h"

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
}

void UFZFHeldItemComponent::HoldItem(UFZFItemData* ItemData)
{
    // 기존에 손에 들고 있던 아이템이 있으면 먼저 제거
    ClearHeldItem();

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
            }
        }
    }
}

void UFZFHeldItemComponent::ClearHeldItem()
{
    // 현재 손에 든 아이템이 있으면 제거
    if (CurrentHeldItem)
    {
        CurrentHeldItem->Destroy();
        CurrentHeldItem = nullptr;
    }
}