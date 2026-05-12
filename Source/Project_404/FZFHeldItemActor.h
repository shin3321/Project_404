#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FZFHeldItemActor.generated.h"

class UStaticMeshComponent;
class UStaticMesh;

UCLASS()
class PROJECT_404_API AFZFHeldItemActor: public AActor
{
    GENERATED_BODY()

public:
    AFZFHeldItemActor();

    // ItemData에서 받은 Mesh를 이 Actor에 적용하는 함수
    void SetHeldMesh(UStaticMesh* InMesh);

private:
    // 실제로 화면에 보일 Static Mesh Component
    // 캐릭터 손에 붙을 때 이 컴포넌트가 보이게 됨
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Held Item", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UStaticMeshComponent> MeshComponent;
};