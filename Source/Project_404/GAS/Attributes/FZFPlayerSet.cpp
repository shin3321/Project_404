#include "GAS/Attributes/FZFPlayerSet.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h" // DOREPLIFETIME 매크로 사용을 위해 필수!
#include "AbilitySystemComponent.h"
#include "GameplayTag/FZFGameplayTags.h"

UFZFPlayerSet::UFZFPlayerSet()
{
	InitMovementSpeed(500.0f);
	InitMaxMovementSpeed(GetMovementSpeed());
	InitStamina(100.0f);
	InitMaxStamina(GetStamina());
	InitAttackSpeed(1.0f);

	// Todo : 무기 데이터로 붙임
	InitAttackRange(500);
	InitAttackRadius(30);
	InitAttack(0.0f);
}

void UFZFPlayerSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Stamina와 MaxStamina의 네트워크 복제를 활성화합니다.
	DOREPLIFETIME(UFZFPlayerSet, Stamina);
	DOREPLIFETIME(UFZFPlayerSet, MaxStamina);
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

bool UFZFPlayerSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	if (!Super::PreGameplayEffectExecute(Data))
	{
		return false;
	}

	// 데미지가 들어오려고 할 때
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		// 무적 상태라면 GE 실행 자체를 여기서 취소(false 반환)하거나 데미지를 0으로 만듦
		if (!CanReceiveDamage())
		{
			Data.EvaluatedData.Magnitude = 0.0f; // 들어오는 데미지 수치를 0으로 조작
			return false; // 더 이상 이 GE를 진행하지 않음
		}
	}

	return true;
}

void UFZFPlayerSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		// 무적이면 데미지 무효화
		if (!CanReceiveDamage())
		{
			SetDamage(0.0f);
			return;
		}

		// 부모에서 이미 HP 삭감 및 클램핑이 끝난 상태로 HUD에 발송
		if (OnHPChanged.IsBound())
		{
			OnHPChanged.Broadcast(GetHP(), GetMaxHP());
		}
	}


	// 자연 치유 추가 : GE에 의해 HP 속성이 직접 올라갔을 때 UI 동기화
	if (Data.EvaluatedData.Attribute == GetHPAttribute())
	{
		// 자연 치유 도중 MaxHP를 넘어서 오버힐 되는 것을 방지하기 위해 다시 한번 안전하게 Clamp
		SetHP(FMath::Clamp(GetHP(), 0.0f, GetMaxHP()));

		// 치유되어 바뀐 현재 체력을 HUD UI에 실시간으로 발송!
		if (OnHPChanged.IsBound())
		{
			// 부모에 구현된 GetHP(), GetMaxHP() 호출
			OnHPChanged.Broadcast(GetHP(), GetMaxHP());
		}
	}
	// 실제 GE가 적용된 후 최종 수치를 다시 한번 제한 (매우 중요)
 	if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		SetStamina(FMath::Clamp(GetStamina(), 0.0f, GetMaxStamina()));

		// 플레이어의 스테미나가 변경 될 때, HUD에 발송
		if (OnStaminaChanged.IsBound())
		{
			OnStaminaChanged.Broadcast(GetStamina(), GetMaxStamina());
		}
	}
}

void UFZFPlayerSet::OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFZFPlayerSet, MaxStamina, OldMaxStamina);

	if (OnStaminaChanged.IsBound())
	{
		OnStaminaChanged.Broadcast(GetStamina(), GetMaxStamina());
	}
}



void UFZFPlayerSet::OnRep_Stamina(const FGameplayAttributeData& OldStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFZFPlayerSet, Stamina, OldStamina);

	if (OnStaminaChanged.IsBound())
	{
		OnStaminaChanged.Broadcast(GetStamina(), GetMaxStamina());
	}
}

// 무적 버프를 가지고 있지 않으면 데미지를 받을 수 있는 상태이다.
bool UFZFPlayerSet::CanReceiveDamage() const
{
	UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();

	if (!ASC)
	{
		return true;
	}

	return !ASC->HasMatchingGameplayTag(FZFGameplayTags::State_Buff_Invincible);
}
