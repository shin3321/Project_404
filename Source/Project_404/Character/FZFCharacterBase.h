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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = GAS)
	// ASC = AbilitySystemComponent
	TObjectPtr<class UFZFAbilitySystemComponent> ASC;

	// 캐릭터의 스텟을 AttributeSet으로 처리
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = GAS)
	TObjectPtr<class UFZFAttributeSet> AttributeSet;

	// 초기에 캐릭터가 가질 GameAbility를 지정
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GAS)
	TArray<TSubclassOf<class UGameplayAbility>> StartupAbilities;

};
