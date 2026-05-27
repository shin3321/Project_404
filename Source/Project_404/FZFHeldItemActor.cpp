#include "FZFHeldItemActor.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraComponent.h"
#include "Item/FZFItemData.h"
#include "Net/UnrealNetwork.h"

AFZFHeldItemActor::AFZFHeldItemActor()
{
    // Tick은 필요 없으니까 꺼둠
    PrimaryActorTick.bCanEverTick = false;

    bReplicates = true;
    SetReplicateMovement(true);

    // Static Mesh Component 생성
    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));

    // 이 Actor의 RootComponent로 설정
   // 나중에 손 소켓에 Attach할 때 이 Root 기준으로 붙음
    SetRootComponent(MeshComponent);

    // 손에 들 아이템은 캐릭터랑 충돌하면 거슬릴 수 있으니까 충돌 끔
    MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    ReadyEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ReadyEffectComponent"));
    ReadyEffectComponent->SetupAttachment(RootComponent);
    ReadyEffectComponent->SetAutoActivate(false);
}

void AFZFHeldItemActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AFZFHeldItemActor, ItemData);
}

void AFZFHeldItemActor::SetHeldItemData(UFZFItemData* NewItemData)
{
    ItemData = NewItemData;
    ApplyItemData();
}

void AFZFHeldItemActor::OnRep_ItemData()
{
    ApplyItemData();
}

void AFZFHeldItemActor::ApplyItemData()
{
    if (!ItemData)
    {
        if (MeshComponent)
        {
            MeshComponent->SetStaticMesh(nullptr);
        }
        if (ReadyEffectComponent)
        {
            ReadyEffectComponent->SetAsset(nullptr);
        }
        return;
    }

    if (MeshComponent)
    {
        MeshComponent->SetStaticMesh(ItemData->Mesh);
    }

    if (ReadyEffectComponent)
    {
        ReadyEffectComponent->SetAsset(ItemData->WeaponReadyEffect);
    }
}

void AFZFHeldItemActor::SetFirstPersonVisualMode()
{
    if (MeshComponent)
    {
        // 로컬 1인칭 전용 아이템.
        // 나(Owner)에게만 보이게 설정.
        MeshComponent->SetOnlyOwnerSee(true);
        MeshComponent->SetOwnerNoSee(false);
    }

    if (ReadyEffectComponent)
    {
        ReadyEffectComponent->SetOnlyOwnerSee(true);
        ReadyEffectComponent->SetOwnerNoSee(false);
    }
}

void AFZFHeldItemActor::SetThirdPersonVisualMode()
{
    if (MeshComponent)
    {
        MeshComponent->SetHiddenInGame(false);
        MeshComponent->SetOwnerNoSee(true);
        MeshComponent->SetOnlyOwnerSee(false);
    }

    if (ReadyEffectComponent)
    {
        ReadyEffectComponent->SetHiddenInGame(false);
        ReadyEffectComponent->SetOwnerNoSee(true);
        ReadyEffectComponent->SetOnlyOwnerSee(false);
    }
}

void AFZFHeldItemActor::ToggleReadyEffect(bool bIsReady)
{
    if (!ReadyEffectComponent || !ReadyEffectComponent->GetAsset())
    {
        return;
    }

    if (bIsReady)
    {
        ReadyEffectComponent->Activate(true);
    }
    else
    {
        ReadyEffectComponent->Deactivate();
    }
}