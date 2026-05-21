// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA/FZFGA_TogglePickaxe.h"
#include "Inventory/FZFHeldItemComponent.h"
#include "Item/ItemTypes.h"
#include "Item/FZFItemData.h"
#include "Item/Equipment/FZFEquipmentItemData.h"
#include "Character/Player/FZFCharacterPlayer.h"

UFZFGA_TogglePickaxe::UFZFGA_TogglePickaxe()
{
	// 인스턴싱 정책: 어빌리티가 실행될 때마다 인스턴스를 생성 (데이터 관리가 편함)
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	// 실행 정책 (반응성 강화)
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UFZFGA_TogglePickaxe::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	// 로직 실행
	AFZFCharacterPlayer* AvatarActor = Cast<AFZFCharacterPlayer>(GetAvatarActorFromActorInfo());
	if (!AvatarActor)
		return;

	// 현재 아이템을 들고 있는지 체크.
	UFZFItemData* ItemData = AvatarActor->GetHeldItemComponent()->GetCurrentItemData();
	if (!IsValid(ItemData))
	{
		UFZFEquipmentItemData* HeldItemData = Cast<UFZFEquipmentItemData>(ItemData);
		
		// 들고있는 아이템이 장비이고 곡괭이라면,
		if (HeldItemData && HeldItemData->GetItemType() == EEquipmentType::Pickaxe)
		{
			AvatarActor->UnEquipPickaxe();

			// 실행 후 즉시 어빌리티 종료
			EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
			return;
		}
	}

	AvatarActor->EquipPickaxe();

	// 실행 후 즉시 어빌리티 종료
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
