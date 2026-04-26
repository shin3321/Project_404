#pragma once

#include "CoreMinimal.h"
#include "Character/FZFCharacterBase.h"
#include "InputActionValue.h"
#include "FZFCharacterPlayer.generated.h"

// 전방선언.
class UInputAction;

UCLASS()
class PROJECT_404_API AFZFCharacterPlayer : public AFZFCharacterBase
{
	GENERATED_BODY()
	
public:
	AFZFCharacterPlayer();

protected:
	virtual void PossessedBy(AController* NewController) override;
	virtual void InitAbilitySystem() override;

	// 입력 장치와 캐릭터 기능을 연결하기 위한 설정 함수
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// 이동 함수
	void Move(const FInputActionValue& Value);

	// 회전 함수
	void Look(const FInputActionValue& Value);

	// 상호작용 함수
	void Interact(const FInputActionValue& Value);

// Camera Section
protected:
	// 카메라 컴포넌트 구성
	UPROPERTY(VisibleAnywhere, Category = Camera)
	TObjectPtr<class UCameraComponent> Camera;


// Input Section
protected:

	UPROPERTY(VisibleAnywhere, Category = Input, BlueprintReadOnly)
	TObjectPtr<class UInputMappingContext> DefaultMappingContext;

	UPROPERTY(VisibleAnywhere, Category = Input, BlueprintReadOnly)
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(VisibleAnywhere, Category = Input, BlueprintReadOnly)
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(VisibleAnywhere, Category = Input, BlueprintReadOnly)
	TObjectPtr<UInputAction> LookAction;
	
	UPROPERTY(VisibleAnywhere, Category = Input, BlueprintReadOnly)
	TObjectPtr<UInputAction> InteractAction;
};
