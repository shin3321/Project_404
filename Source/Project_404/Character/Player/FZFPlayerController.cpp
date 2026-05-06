#include "Character/Player/FZFPlayerController.h"
#include "Shop/FZFStore.h"
#include "Net/UnrealNetwork.h"

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

void AFZFPlayerController::ServerRequestPurchase_Implementation(AFZFStore* TargetStore, FName ItemId, float ItemCost)
{
	TargetStore->ProcessPurchase(this, ItemId, ItemCost);
}
