// Fill out your copyright notice in the Description page of Project Settings.


#include "FZFGA_ReviveStone.h"
#include "Character/Player/FZFCharacterPlayer.h"
#include "Inventory/FZFInventoryComponent.h"
#include "Game/FZFGameMode.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"

UFZFGA_ReviveStone::UFZFGA_ReviveStone()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UFZFGA_ReviveStone::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// 서버에서만 실행
	if (!HasAuthority(&ActivationInfo))
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	AFZFGameMode* GameMode = Cast<AFZFGameMode>(World->GetAuthGameMode());
	if (!GameMode)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	TArray<APlayerController*> DeadPlayers;
	for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (PC)
		{
			// 폰이 없거나, 폰이 있지만 죽은 상태인 경우를 체크
			APawn* Pawn = PC->GetPawn();
			bool bIsDead = false;
			if (Pawn == nullptr)
			{
				bIsDead = true;
			}
			else if (AFZFCharacterBase* Character = Cast<AFZFCharacterBase>(Pawn))
			{
				if (Character->IsDead())
				{
					bIsDead = true;
				}
			}

			if (bIsDead)
			{
				DeadPlayers.Add(PC);
			}
		}
	}

	if (DeadPlayers.Num() > 0)
	{
		// 랜덤으로 한 명 선택
		int32 RandomIndex = FMath::RandRange(0, DeadPlayers.Num() - 1);
		APlayerController* TargetPC = DeadPlayers[RandomIndex];

		// 부활 처리 (RestartPlayer가 새로운 폰을 스폰하고 빙의시킴)
		GameMode->RestartPlayer(TargetPC);

		// 아이템 소모
		if (AFZFCharacterPlayer* OwnerCharacter = Cast<AFZFCharacterPlayer>(GetAvatarActorFromActorInfo()))
		{
			if (UFZFInventoryComponent* Inventory = OwnerCharacter->GetInventoryComponent())
			{
				Inventory->RemoveSelectedItem();
			}
		}

		// 성공적으로 수행됨
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
	else
	{
		// 죽은 사람이 없음 - 아무 일도 일어나지 않음
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
}

void UFZFGA_ReviveStone::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
