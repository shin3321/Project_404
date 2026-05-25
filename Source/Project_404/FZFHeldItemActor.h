#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FZFHeldItemActor.generated.h"

class UStaticMeshComponent;
class UStaticMesh;
class UFZFItemData;
class UNiagaraComponent;

UCLASS()
class PROJECT_404_API AFZFHeldItemActor: public AActor
{
    GENERATED_BODY()

public:
    AFZFHeldItemActor();

protected:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


public:

    FORCEINLINE UStaticMeshComponent* GetItemMeshComponent() const { return MeshComponent; }

    void SetHeldItemData(UFZFItemData* NewItemData);

    void SetFirstPersonVisualMode();
    void SetThirdPersonVisualMode();

    void ToggleReadyEffect(bool bIsReady);

protected:
    UFUNCTION()
    void OnRep_ItemData();

    void ApplyItemData();

private:
    // 실제로 화면에 보일 Static Mesh Component
    // 캐릭터 손에 붙을 때 이 컴포넌트가 보이게 됨
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Held Item", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UStaticMeshComponent> MeshComponent;

    // 쿨타임 완료 시 반짝이는 파티클을 재생할 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Held Item", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UNiagaraComponent> ReadyEffectComponent;

    UPROPERTY(ReplicatedUsing = OnRep_ItemData)
    TObjectPtr<UFZFItemData> ItemData;
};