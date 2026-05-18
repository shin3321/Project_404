#include "GAS/Attributes/FZFAttributeSet.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayEffectExtension.h"

// 추후 인터페이스로 뺄 헤더들
#include "Inventory/FZFHeldItemComponent.h"
#include "Item/FZFItemData.h"
#include "Character/Monster/FZFMonster.h"
#include "Character/Monster/MonsterData/FZFMonsterData.h"

UFZFAttributeSet::UFZFAttributeSet()
{
	InitHP(100.0f);
	InitMaxHP(100.0f);
}

void UFZFAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHPAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHP());
	}
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

void UFZFAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	UE_LOG(LogTemp, Warning,
		TEXT("[AttrDebug] Owner=%s / Avatar=%s / Instigator=%s / Attr=%s / HP=%.1f / MaxHP=%.1f / Damage=%.1f"),
		*GetNameSafe(GetOwningActor()),
		*GetNameSafe(GetOwningAbilitySystemComponent()
			? GetOwningAbilitySystemComponent()->GetAvatarActor()
			: nullptr),
		*GetNameSafe(Data.EffectSpec.GetContext().GetInstigator()),
		*Data.EvaluatedData.Attribute.GetName(),
		GetHP(),
		GetMaxHP(),
		GetDamage()
	);

	// 이펙트를 유발한 가해자(공격자)의 정보와 GE 정보 추출
	AActor* InstigatorActor = Data.EffectSpec.GetContext().GetInstigator();
	const UGameplayEffect* AppliedGE = Data.EffectSpec.Def;

	if (!InstigatorActor || !AppliedGE)
	{
		return;
	}

	// 공격자가 가질 수 있는 합법적인 이펙트 목록을 가져올 임시 배열
	TArray<TSubclassOf<UGameplayEffect>> AllowedEffects;

	if (UFZFHeldItemComponent* HeldItemComp = InstigatorActor->FindComponentByClass<UFZFHeldItemComponent>())
	{
		if (UFZFItemData* ItemData = HeldItemComp->GetCurrentItemData())
		{
			// 데이터 에셋을 TArray로 바꿈
			AllowedEffects = ItemData->AllowedEffectClasses;
		}
	}
	else if (AFZFMonster* Monster = Cast<AFZFMonster>(InstigatorActor))
	{
		if (UFZFMonsterData* MData = Monster->GetMonsterData())
		{
			// 데이터 에셋을 TArray로 바꿈
			AllowedEffects = MData->AllowedEffectClasses;
		}
	}

	// 현재 들어온 이펙트가 검증된 이펙트 목록에 있는지 확인
	bool bIsValidEffect = false;
	for(const auto& EffectClass : AllowedEffects)
	{
		if (EffectClass && AppliedGE->GetClass() == EffectClass)
		{
			bIsValidEffect = true;
			break;
		}
	}

	// 검증 실패 시, 수치 무효화 및 차단
	if (!bIsValidEffect)
	{
		if (Data.EvaluatedData.Attribute == GetDamageAttribute())
		{
			SetDamage(0.0f);
			return;
		}
	}

	// 검증 성공 시, Attribute별 실제 처리
	// 데미지 처리
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		const float LocalDamage = GetDamage();
		// 값 넣어주자 마자 리셋시킴
		SetDamage(0.0f);

		if (LocalDamage > 0.0f)
		{
			const float NewHP = GetHP() - LocalDamage;
			SetHP(FMath::Clamp(NewHP,0.0f,GetMaxHP()));
		
			// TODO : 사망 처리 로직
		}
		else
		{
			
		}		
	}

	// TODO : 다른 어트리뷰트 처리 로직 (힐 , 버프 등)
	// else if(Data.EvaluatedData.Attribute == GetHealAttribute())
	//{
		// 힐 로직 처리
	//}	
}
