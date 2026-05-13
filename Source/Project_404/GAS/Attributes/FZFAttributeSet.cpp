#include "GAS/Attributes/FZFAttributeSet.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UFZFAttributeSet::UFZFAttributeSet()
{
	InitHP(100.0f);
	InitMaxHP(100.0f);
}

void UFZFAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

}

void UFZFAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeBaseChange(Attribute, OldValue, NewValue);

	// 이동 속도 어트리뷰트가 변경되었는지 확인
	if (Attribute == GetMovementSpeedAttribute())
	{
		// 아바타 액터(캐릭터)를 가져와 MaxWalkSpeed 업데이트
		if (ACharacter* Character = Cast<ACharacter>(GetOwningAbilitySystemComponent()->GetAvatarActor()))
		{
			// GE에 의해 계산된 최종 결과값(NewValue)를 Character의 MovementComp(MaxWalkSpeed)에 적용
			Character->GetCharacterMovement()->MaxWalkSpeed = NewValue;
		}
	}
}
