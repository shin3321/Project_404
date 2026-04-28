// TestCameraPawn.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "FZFInventoryComponent.h"
#include "Item/FZFItemBase.h"

#include "TestCameraPawn.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UFloatingPawnMovement;

UCLASS()
class PROJECT_404_API ATestCameraPawn : public APawn
{
    GENERATED_BODY()

public:
    ATestCameraPawn();

protected:
    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
    virtual void Tick(float DeltaTime) override;

    void MoveForward(float Value);
    void MoveRight(float Value);
    void Turn(float Value);
    void LookUp(float Value);

    // 상호작용 입력 시 현재 바라보는 아이템을 획득하는 함수
    void TraceItem();





protected:
    UPROPERTY(VisibleAnywhere)
    USceneComponent* Root;

    UPROPERTY(VisibleAnywhere)
    USpringArmComponent* SpringArm;

    UPROPERTY(VisibleAnywhere)
    UCameraComponent* Camera;

    UPROPERTY(VisibleAnywhere)
    UFloatingPawnMovement* Movement;

    // 현재 카메라로 바라보고 있는 아이템 액터
    UPROPERTY()
    TObjectPtr<AFZFItemBase> CurrentTargetItem;

    //플레이어 인벤토리 데이터 관리 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UFZFInventoryComponent* InventoryComponent;
};