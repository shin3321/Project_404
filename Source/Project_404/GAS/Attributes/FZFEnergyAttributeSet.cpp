// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Attributes/FZFEnergyAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"
#include "GameplayTag/FZFGameplayTags.h"
#include "Boss/FZFEnergyRelay.h"

UFZFEnergyAttributeSet::UFZFEnergyAttributeSet()
{
	// Todo: 추후 HP 10으로 변경
	// 기본값 초기화
	InitHP(5.0f); 
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

		// 어빌리티에서 AppendDynamicAssetTags로 넣어준 태그 주머니를 통째로 가져옵니다.
		FGameplayTagContainer AssetTags = Data.EffectSpec.DynamicAssetTags;

		// 디버그용 로그: 어떤 무기 태그가 묻어 들어왔는지 확인
		UE_LOG(LogTemp, Log, TEXT("[RelayAttr] 동적 주입된 무기 태그: %s"), *AssetTags.ToString());

		FGameplayTag GunTag = FZFGameplayTags::Ability_Action_Attack_Rifle;
		FGameplayTag PickaxeTag = FZFGameplayTags::Ability_Action_Attack_Pickaxe;
		FGameplayTag RobotTag = FZFGameplayTags::Ability_Action_Attack_Robot;

		// 조건 검사: 곡괭이 또는 로봇 공격이 맞는지 확인
		if (AssetTags.HasTag(PickaxeTag) || AssetTags.HasTag(RobotTag) || AssetTags.HasTag(GunTag))
		{
			if (LocalDamage > 0.0f)
			{
				// 데미지 로직 적용
				const float NewHP = FMath::Max(GetHP() - LocalDamage, 0.0f);
				SetHP(NewHP);

				UE_LOG(LogTemp, Warning, TEXT("[RelayAttr] 데미지 적중! Damage=%f | 남은 HP=%f"),
					LocalDamage,
					GetHP());

				// 체력이 0이 되었을 때 보스 연동 처리
				if (NewHP <= 0.0f)
				{
					AActor* OwnerActor = Data.Target.GetAvatarActor();
					if (OwnerActor)
					{
						// 동력원 액터로 캐스팅
						AFZFEnergyRelay* EnergyRelay = Cast<AFZFEnergyRelay>(OwnerActor);
						if (EnergyRelay)
						{
							// 보스 피 깎기 및 고리 파괴 함수 호출
							EnergyRelay->HandleDead();
						}
					}
				}
			}
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
