#pragma once

#include "CoreMinimal.h"
#include "Character/FZFCharacterBase.h"
#include "InputActionValue.h"
#include "FZFCharacterPlayer.generated.h"

// 전방선언.
class UInputAction;
class UInputMappingContext;
class UFZFInventoryComponent;


UCLASS()
class PROJECT_404_API AFZFCharacterPlayer : public AFZFCharacterBase
{
	GENERATED_BODY()
	
public:
	AFZFCharacterPlayer();

public:
	// 인벤토리 컴포넌트에 접근할 수 있도록 Getter 추가 (GA에서 사용)
	UFZFInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }
	
	// 카메라에 안전하게 접근할 수 있도록 Getter 추가 (GA에서 사용)
	FORCEINLINE class UCameraComponent* GetCamera() const { return Camera; }

protected:
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void InitAbilitySystem() override;

	// 입력 장치와 캐릭터 기능을 연결하기 위한 설정 함수
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// 입력 매핑 컨텍스트(IMC)를 활성화하여 캐릭터의 조작 체계를 설정
	void ApplyMappingContext(UInputMappingContext* InMappingContext);
protected:
	// 이동 함수
	void Move(const FInputActionValue& Value);

	// 회전 함수
	void Look(const FInputActionValue& Value);

	// 상호작용 함수
	void Interact();

// Camera Section
	// 네트워크 상태 동기화 함수
	virtual void OnRep_PlayerState() override;

protected:
	// 카메라 컴포넌트 구성
	UPROPERTY(VisibleAnywhere, Category = Camera)
	TObjectPtr<class UCameraComponent> Camera;


// Input Section
protected:

	UPROPERTY(VisibleAnywhere, Category = Input, BlueprintReadOnly)
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(VisibleAnywhere, Category = Input, BlueprintReadOnly)
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(VisibleAnywhere, Category = Input, BlueprintReadOnly)
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(VisibleAnywhere, Category = Input, BlueprintReadOnly)
	TObjectPtr<UInputAction> LookAction;
	
	UPROPERTY(VisibleAnywhere, Category = Input, BlueprintReadOnly)
	TObjectPtr<UInputAction> InteractAction;

// Interact Section
protected:
	// 상호작용하면 UI로 표시하기 위한 함수
	void DetectInteractable();

protected:
	// 현재 카메라 조준점에 들어와 있는 아이템 (UI 표시용)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	TWeakObjectPtr<class AFZFItemBase> CurrentTargetItem;

// Inventory Section
protected:
	// 플레이어 인벤토리 데이터 관리 컴포넌트 추가
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	TObjectPtr<UFZFInventoryComponent> InventoryComponent;

};
