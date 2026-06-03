#include "Character/Player/FZFPlayerState.h"
#include "GAS/FZFAbilitySystemComponent.h"
#include "GAS/Attributes/FZFPlayerSet.h"
#include "Net/UnrealNetwork.h"
#include "Inventory/FZFInventoryComponent.h"


AFZFPlayerState::AFZFPlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

	ASC = CreateDefaultSubobject<UFZFAbilitySystemComponent>("ASC");

	// ASC 자체가 네트워크를 통해 동기화되도록 켬
	ASC->SetIsReplicated(true);

	// 플레이어 컨트롤 캐릭터에 최적화된 혼합 모드 설정
	// 나 자신: 서버가 모든 데이터를 나에게 동기화
	// 다른 플레이어: 내 화면에 보이는 다른 사람의 데이터는 최소한의 정보만 동기화
	ASC->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	PlayerSet = CreateDefaultSubobject<UFZFPlayerSet>("PlayerSet");

	// (선택) AttributeSet도 여기서 생성
	//AttributeSet = CreateDefaultSubobject<UFZFAttributeSet>(TEXT("AttributeSet"));

	InventoryItemIds.SetNum(MaxItemCount);

	bReplicates = true;
	//NetUpdateFrequency = 30.0f;
	//MinNetUpdateFrequency = 10.0f;
}

void AFZFPlayerState::OnRep_InventoryItemIds()
{
	UE_LOG(LogTemp, Warning, TEXT("=== CLIENT OnRep_InventoryItemIds ==="));
	UE_LOG(LogTemp, Warning, TEXT("PS: %s Authority=%d"), *GetName(), HasAuthority());
	UE_LOG(LogTemp, Warning, TEXT("Pawn: %s"), *GetNameSafe(GetPawn()));
	UE_LOG(LogTemp, Warning, TEXT("InventoryItemIds Num=%d"), InventoryItemIds.Num());

	for (int32 i = 0; i < InventoryItemIds.Num(); ++i)
	{
		UE_LOG(LogTemp, Warning, TEXT("Client PS Slot[%d] = %s"),
			i,
			*InventoryItemIds[i].ToString());
	}

    NotifyInventoryChanged();
}

void AFZFPlayerState::OnRep_SelectedSlotIndex()
{
    NotifyInventoryChanged();
}

void AFZFPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFZFPlayerState, InventoryItemIds);
	DOREPLIFETIME(AFZFPlayerState, SelectedSlotIndex);
}

UAbilitySystemComponent* AFZFPlayerState::GetAbilitySystemComponent() const
{
	return ASC;
}

UFZFPlayerSet* AFZFPlayerState::GetPlayerSet() const
{
	return PlayerSet;
}

bool AFZFPlayerState::AddItemId(FName InItemId)
{
	UE_LOG(LogTemp, Warning, TEXT("PS AddItemId Called. Authority=%d ItemId=%s Num=%d"),
		HasAuthority(),
		*InItemId.ToString(),
		InventoryItemIds.Num());

	if (!HasAuthority())
	{
		return false;
	}

	if (InItemId.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("PS AddItemId Failed: ItemId is None"));
		return false;
	}

	if (InventoryItemIds.Num() != MaxItemCount)
	{
		InventoryItemIds.SetNum(MaxItemCount);
	}

	for (int32 i = 0; i < InventoryItemIds.Num(); ++i)
	{
		if (InventoryItemIds[i].IsNone())
		{
			InventoryItemIds[i] = InItemId;

			UE_LOG(LogTemp, Warning, TEXT("PS After Set Slot[%d]=%s Replicates=%d NetDormancy=%d NetUpdateFrequency=%f"),
				i,
				*InventoryItemIds[i].ToString(),
				GetIsReplicated(),
				(int32)NetDormancy,
				NetUpdateFrequency);

			NotifyInventoryChanged();
			//FlushNetDormancy();
			//ForceNetUpdate();

			return true;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("PS AddItemId Failed: No Empty Slot"));
	return false;
}

void AFZFPlayerState::RemoveItemAt(int32 SlotIndex)
{
	if (!HasAuthority())
		return;

	if (!InventoryItemIds.IsValidIndex(SlotIndex))
		return;

	InventoryItemIds[SlotIndex] = NAME_None;

	NotifyInventoryChanged();
	ForceNetUpdate();
}

FName AFZFPlayerState::GetItemIdAt(int32 SlotIndex) const
{
	if (!InventoryItemIds.IsValidIndex(SlotIndex))
	{
		return NAME_None;
	}

	return InventoryItemIds[SlotIndex];
}

void AFZFPlayerState::SetSelectedSlotIndex(int32 InSlotIndex)
{
	if (!HasAuthority())
	{
		return;
	}

	if (InSlotIndex < 0 || InSlotIndex >= MaxItemCount)
	{
		SelectedSlotIndex = -1;
	}
	else
	{
		SelectedSlotIndex = InSlotIndex;
	}

	NotifyInventoryChanged();
	ForceNetUpdate();
}

void AFZFPlayerState::NotifyInventoryChanged()
{
	UE_LOG(LogTemp, Warning, TEXT("=== PS NotifyInventoryChanged ==="));
	UE_LOG(LogTemp, Warning, TEXT("PS: %s Authority=%d"), *GetName(), HasAuthority());
	UE_LOG(LogTemp, Warning, TEXT("Pawn: %s"), *GetNameSafe(GetPawn()));
	UE_LOG(LogTemp, Warning, TEXT("InventoryItemIds Num: %d"), InventoryItemIds.Num());

	for (int32 i = 0; i < InventoryItemIds.Num(); ++i)
	{
		UE_LOG(LogTemp, Warning, TEXT("PS Slot[%d] = %s"),
			i,
			*InventoryItemIds[i].ToString());
	}

	APawn* Pawn = GetPawn();
	if (!Pawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("NotifyInventoryChanged Failed: Pawn is null"));
		return;
	}

	UFZFInventoryComponent* InventoryComponent = Pawn->FindComponentByClass<UFZFInventoryComponent>();
	UE_LOG(LogTemp, Warning, TEXT("InventoryComponent: %s"), *GetNameSafe(InventoryComponent));

	if (InventoryComponent)
	{
		InventoryComponent->SyncFromPlayerState();
	}
}

void AFZFPlayerState::CopyProperties(APlayerState* NewPlayerState)
{
	Super::CopyProperties(NewPlayerState);

	UE_LOG(LogTemp, Warning, TEXT("=== AFZFPlayerState::CopyProperties ==="));
	UE_LOG(LogTemp, Warning, TEXT("Old PS: %s"), *GetName());
	UE_LOG(LogTemp, Warning, TEXT("New PS: %s"), *GetNameSafe(NewPlayerState));

	AFZFPlayerState* NewFZFPS = Cast<AFZFPlayerState>(NewPlayerState);
	if (!NewFZFPS)
	{
		UE_LOG(LogTemp, Warning, TEXT("CopyProperties Failed: NewPlayerState is not AFZFPlayerState"));
		return;
	}

	NewFZFPS->InventoryItemIds = InventoryItemIds;
	NewFZFPS->SelectedSlotIndex = SelectedSlotIndex;
	NewFZFPS->MaxItemCount = MaxItemCount;

	UE_LOG(LogTemp, Warning, TEXT("Copied InventoryItemIds Num=%d"), InventoryItemIds.Num());

	for (int32 i = 0; i < InventoryItemIds.Num(); ++i)
	{
		UE_LOG(LogTemp, Warning, TEXT("Copy Slot[%d] = %s"),
			i,
			*InventoryItemIds[i].ToString());
	}
}