#include "FZFHeldItemActor.h"
#include "Components/StaticMeshComponent.h"

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
    MeshComponent->SetIsReplicated(true);
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
        return;
    }

    if (!MeshComponent)
    {
        return;
    }

    MeshComponent->SetStaticMesh(ItemData->Mesh);
}

void AFZFHeldItemActor::SetFirstPersonVisualMode()
{
    if (!MeshComponent)
    {
        return;
    }

    // 로컬 1인칭 전용 아이템.
    MeshComponent->SetOnlyOwnerSee(false);
    MeshComponent->SetOwnerNoSee(false);
}

void AFZFHeldItemActor::SetThirdPersonVisualMode()
{
    if (!MeshComponent)
    {
        return;
    }

    // 3인칭 복제 아이템.
    // Owner에게는 보이지 않게 해서, 자기 화면에서는 1인칭 ArmMesh용 아이템만 보이게 함.
    MeshComponent->SetOwnerNoSee(true);
    MeshComponent->SetOnlyOwnerSee(false);
}