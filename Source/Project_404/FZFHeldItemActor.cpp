#include "FZFHeldItemActor.h"
#include "Components/StaticMeshComponent.h"

AFZFHeldItemActor::AFZFHeldItemActor()
{
    // Tick은 필요 없으니까 꺼둠
    PrimaryActorTick.bCanEverTick = false;

    // Static Mesh Component 생성
    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));

    // 이 Actor의 RootComponent로 설정
    // 나중에 손 소켓에 Attach할 때 이 Root 기준으로 붙음
    RootComponent = MeshComponent;

    // 손에 들 아이템은 캐릭터랑 충돌하면 거슬릴 수 있으니까 충돌 끔
    MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AFZFHeldItemActor::SetHeldMesh(UStaticMesh* InMesh)
{
    // Mesh가 없으면 아무것도 하지 않음
    if (!InMesh)
    {
        return;
    }

    // ItemData에서 받은 Mesh를 StaticMeshComponent에 적용
    MeshComponent->SetStaticMesh(InMesh);
}