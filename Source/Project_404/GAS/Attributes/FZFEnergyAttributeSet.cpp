// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Attributes/FZFEnergyAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"
#include "GameplayTag/FZFGameplayTags.h"

UFZFEnergyAttributeSet::UFZFEnergyAttributeSet()
{
	// 기본값 초기화
	InitHP(10.0f);
	InitMaxHP(GetHP());

	InitDamage(0.0f);
}

void UFZFEnergyAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// COND_None: 조건 없이 항상 모든 클라이언트에게 동기화
	DOREPLIFETIME_CONDITION_NOTIFY(UFZFEnergyAttributeSet, HP, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UFZFEnergyAttributeSet, MaxHP, COND_None, REPNOTIFY_Always);
}

void UFZFEnergyAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	// Gameplay Effect를 통해 'Damage' 속성이 들어왔을 때 처리
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		// 들어온 임시 데미지 양을 로컬 변수에 저장
		const float LocalDamage = GetDamage();

		// 데미지 버퍼 변수는 누적되지 않도록 즉시 0
		SetDamage(0.0f);

		// 들어온 이펙트의 Asset Tags 확인
		// => GE : Component -> AssetTags에 추가할 수 있음
		FGameplayTagContainer AssetTags;
		Data.EffectSpec.GetAllAssetTags(AssetTags);

		FGameplayTag PickaxeTag = FZFGameplayTags::Ability_Action_Attack_Pickaxe;
		FGameplayTag RobotTag = FZFGameplayTags::Ability_Action_Attack_Robot;

		// 조건 검사: 곡괭이 또는 로봇 공격이 맞는지 확인
		if (AssetTags.HasTag(PickaxeTag) || AssetTags.HasTag(RobotTag))
		{
			// 유효한 공격이므로 실제 HP를 차감하고, 0 아래로 내려가지 않게 Clamping
			const float NewHP = FMath::Max(GetHP() - LocalDamage, 0.0f);
			SetHP(NewHP);

			// 4. 체력이 0 이하(정확히 0)가 되었을 때 보스 연동 처리
			if (NewHP <= 0.0f)
			{
				AActor* OwnerActor = Data.Target.GetAvatarActor();
				if (OwnerActor)
				{
					// 동력원 액터로 캐스팅

					// 보스 피 깎기 및 고리 파괴 함수 호출
				}
			}
		}
		else
		{
			// 곡괭이나 로봇이 아니면 무시
			// HP를 건드리지 않으므로 아무런 이펙트 변화 없이 데미지가 공중분해
		}
	}
}

void UFZFEnergyAttributeSet::OnRep_HP(const FGameplayAttributeData& OldHealth)
{
	// 클라이언트 단에서 GAS 체력 값이 변했음을 인지하고 UI 등을 갱신할 수 있게 만듦
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFZFEnergyAttributeSet, HP, OldHealth);
}

void UFZFEnergyAttributeSet::OnRep_MaxHP(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFZFEnergyAttributeSet, MaxHP, OldMaxHealth);
}
