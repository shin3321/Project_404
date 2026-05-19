#include "GAS/Attributes/FZFPlayerSet.h"
#include "GameplayEffectExtension.h"

UFZFPlayerSet::UFZFPlayerSet()
{
	InitMovementSpeed(300.0f);
	InitStamina(100.0f);
	InitMaxStamina(GetStamina());
	InitAttackSpeed(1.0f);

	// Todo : 무기 데이터
	InitAttackRange(800);
	InitAttackRadius(30);
	InitAttack(50);
}

void UFZFPlayerSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	// 스태미나가 변경될 때 실행
	if (Attribute == GetStaminaAttribute())
	{
		// NewValue(변화하려는 값)를 0과 MaxStamina 사이로 고정
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxStamina());
	}
}

void UFZFPlayerSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	// 실제 GE가 적용된 후 최종 수치를 다시 한번 제한 (매우 중요)
 	if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		SetStamina(FMath::Clamp(GetStamina(), 0.0f, GetMaxStamina()));
	}
}