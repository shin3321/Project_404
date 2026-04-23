#pragma once

#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "FZFAttributeSet.generated.h"

/**
 * [GAS 어트리뷰트 접근자 매크로]
 * AttributeSet에서 각 어트리뷰트(속성)에 대한 Getter, Setter, Init 함수를 자동으로 생성.
 * - GAMEPLAYATTRIBUTE_PROPERTY_GETTER : 어트리뷰트 자체의 속성 정보(FProperty)를 가져옵니다. (주로 이펙트 처리 시 사용)
 * - GAMEPLAYATTRIBUTE_VALUE_GETTER    : 어트리뷰트의 현재 '수치(Value)'를 가져옵니다.
 * - GAMEPLAYATTRIBUTE_VALUE_SETTER    : 어트리뷰트의 수치를 설정합니다. (Clamp 등 보정 로직이 포함된 안전한 수정)
 * - GAMEPLAYATTRIBUTE_VALUE_INITTER   : 어트리뷰트를 초기값으로 설정합니다. (게임 시작 시나 리스폰 시 초기화용)
 */

// [Solve] : 매크로뒤에 백슬래쉬에 공백 허용 시, 매크로 인식 안됨
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class PROJECT_404_API UFZFAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UFZFAttributeSet();

public:
	ATTRIBUTE_ACCESSORS(UFZFAttributeSet, HP);
	ATTRIBUTE_ACCESSORS(UFZFAttributeSet, MaxHP);
	ATTRIBUTE_ACCESSORS(UFZFAttributeSet, MovementSpeed);
	ATTRIBUTE_ACCESSORS(UFZFAttributeSet, MaxMovementSpeed);

	// FGameplayAttributeData는 구조체로 BaseValue와	CurrentValue를 가지고 있다.
	// BaseValue : 순수한 기본값
	// CurrentValue : 현재 적용된 모든 효과가 계산된 결과값
	// Max Data를 넣는 이유? 
	// GAS의 설계 철학은 "영구적인 변화가 아니면 BaseValue를 직접 건드리지 않는다"
	// If) 저주로 최대체력이 깎이는 디버프가 걸린다 -> MaxHp의 CurrentValue를 수정하는게 옳은 방향

private:
	// private : 직접 접근해서 값을 수정하는 것을 방지
	// AllowPrivateAccess : 에디터에서는 값을 확인하고 초기값 설정 허용
	UPROPERTY(BlueprintReadOnly, Category = Stat, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData HP;

	UPROPERTY(BlueprintReadOnly, Category = Stat, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData MaxHP;
	
	UPROPERTY(BlueprintReadOnly, Category = Stat, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData MovementSpeed;

	UPROPERTY(BlueprintReadOnly, Category = Stat, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData MaxMovementSpeed;

	
};
