#include "Character/Player/FZFPlayerController.h"
#include "Shop/FZFStore.h"
#include "Net/UnrealNetwork.h"
#include "FZFPlayerState.h"
#include "Game/FZFGameState.h"
#include "Character/FZFCharacterBase.h"

#include "Manager/FZFSpawnManager.h"
#include "Kismet/GameplayStatics.h"

AFZFPlayerController::AFZFPlayerController()
{
}

void AFZFPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AFZFPlayerController::RequestPurchase(AFZFStore* TargetStore, FName ItemId, float ItemCost)
{
	ServerRequestPurchase(TargetStore, ItemId, ItemCost);
}

void AFZFPlayerController::LoadAndSyncPlayerData()
{
	// 내 컴퓨터에서 돌아가는 컨트롤러일 때만 실행
	if (IsLocalController())
	{
	}
}

void AFZFPlayerController::ChangeSpectateTarget(int32 Direction)
{
	TArray<APawn*> Targets = GetSpectatablePawns();
	if (Targets.Num() == 0) return;

	SpectateIndex = (SpectateIndex + Direction + Targets.Num()) % Targets.Num();

	if (Targets.IsValidIndex(SpectateIndex))
	{
		SetViewTargetWithBlend(Targets[SpectateIndex], 0.2f);
	}
}

void AFZFPlayerController::RequestSpawnItem(FName ItemId, FVector ItemSpawnLocation)
{
	ServerSpawnItem_Implementation(ItemId, ItemSpawnLocation);
}

bool AFZFPlayerController::ServerSpawnItem_Validate(FName ItemId, FVector ItemSpawnLocation)
{
	return !ItemId.IsNone();
}

void AFZFPlayerController::ServerSpawnItem_Implementation(FName ItemId, FVector ItemSpawnLocation)
{
	SpawnManager = Cast<AFZFSpawnManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AFZFSpawnManager::StaticClass()));
	if (SpawnManager)
		SpawnManager->ServerSpawnItem(ItemId, ItemSpawnLocation);

}



TArray<APawn*> AFZFPlayerController::GetSpectatablePawns()
{
	TArray<APawn*> FoundPawns;
	if (AGameStateBase* GS = GetWorld()->GetGameState())
	{
		for (APlayerState* PS : GS->PlayerArray)
		{
			if (PS && PS != GetPlayerState<APlayerState>())
			{
				if (APawn* P = PS->GetPawn())
				{
					if (AFZFCharacterBase* BaseChar = Cast<AFZFCharacterBase>(P))
					{
						// 아까 만든 IsDead() 함수(State.Dead 태그 검사)를 활용
						// if (!BaseChar->IsDead())
						// {
						// 	FoundPawns.Add(P);
						// }
					}
				}
			}
		}
	}
	return FoundPawns;

}

// 서버 로직을 통과하기 전 검증 함수 
bool AFZFPlayerController::ServerRequestPurchase_Validate(AFZFStore* TargetStore, FName ItemId, float ItemCost)
{
	if (ItemCost < 0.0f)
	{
		return false;
	}

	if (TargetStore == nullptr)
	{
		return false;
	}

	return true;
}

void AFZFPlayerController::ServerRequestPurchase_Implementation(AFZFStore* TargetStore, FName ItemId,
	float ItemCost)
{
	TargetStore->ProcessPurchase(this, ItemId, ItemCost);
}
