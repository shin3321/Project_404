#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "FZFCharacterBase.generated.h"

UCLASS()
class PROJECT_404_API AFZFCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// 생성자
	AFZFCharacterBase();

public:
	// ASC 인터페이스 함수 
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// CharacterBase를 상속받은 클래스들의 ASC를 초기화 시키는 함수
	virtual void InitAbilitySystem();

protected:
	// 읽기 가능, 수정 불가
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	// ASC = AbilitySystemComponent
	TObjectPtr<class UFZFAbilitySystemComponent> ASC;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UFZFAttributeSet> AttributeSet;
};
