
#include "FZFInventoryComponent.h"
#include "FZFInventoryWidget.h"
#include "Blueprint/UserWidget.h"
#include "FZFHeldItemComponent.h"
#include "Character/Player/FZFCharacterPlayer.h"
#include "Character/Player/FZFPlayerController.h"
#include "Item/FZFItemData.h"
#include "Item/FZFItemBase.h"
#include "Kismet/GameplayStatics.h"
#include "Manager/FZFSpawnManager.h"

#include "Net/UnrealNetwork.h"

// 인벤토리 컴포넌트 생성자
UFZFInventoryComponent::UFZFInventoryComponent()
{
    // 이 컴포넌트는 Tick 사용 안 함
    PrimaryComponentTick.bCanEverTick = false;

    SetIsReplicatedByDefault(true);
    InventoryItems.SetNum(MaxItemCount);
}

void UFZFInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UFZFInventoryComponent, InventoryItems);
}

void UFZFInventoryComponent::OnRep_InventoryItems()
{
    if (InventoryWidget)
    {
        InventoryWidget->RefreshInventory(InventoryItems, MaxItemCount, SelectedSlotIndex);
    }
}

void UFZFInventoryComponent::InitializeComponent()
{
    Super::InitializeComponent();
    SpawnManager = Cast<AFZFSpawnManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AFZFSpawnManager::StaticClass()));
}


bool UFZFInventoryComponent::AddItem(UFZFItemData* InItemData)
{
    // 서버에서만 실제 데이터를 추가하도록 강제
    if (!GetOwner()->HasAuthority())
    {
        return true; // 클라이언트에서는 일단 성공한 것처럼 리턴 (상호작용 종료를 위해)
    }

    if (!InItemData)
    {
        return false;
    }

    int32 EmptySlotIndex = -1;

    // 빈 슬롯 찾기
    for (int32 i = 0; i < MaxItemCount; ++i)
    {
        if (InventoryItems[i] == nullptr)
        {
            EmptySlotIndex = i;
            break;
        }
    }

    // 빈 슬롯 없으면 추가 실패
    if (EmptySlotIndex == -1)
    {
        return false;
    }

    // 서버에서 배열 수정 -> 자동으로 클라이언트에 복제됨
    InventoryItems[EmptySlotIndex] = InItemData;

    // 서버(리스닝 서버 호스트)의 UI 갱신 및 데이터 복제 트리거를 위해 직접 호출
    OnRep_InventoryItems();

    // 현재 선택된 슬롯에 아이템이 들어왔으면 바로 손에 들기
    if (SelectedSlotIndex == EmptySlotIndex)
    {
        UpdateHeldItemBySelectedSlot();
    }

    return true;
}

// 인벤토리 위젯을 화면에 표시하는 함수
void UFZFInventoryComponent::ShowInventory()
{
    // 로컬 플레이어인 경우에만 위젯 생성 및 표시
    APawn* OwnerPawn = Cast<APawn>(GetOwner());
    if (!OwnerPawn || !OwnerPawn->IsLocallyControlled())
    {
        return;
    }

    // 생성할 인벤토리 위젯 클래스가 없으면 종료
    if (!InventoryWidgetClass)
    {
        return;
    }

    // 인벤토리 위젯이 아직 없으면 생성
    if (!InventoryWidget)
    {
        InventoryWidget = CreateWidget<UFZFInventoryWidget>(GetWorld(), InventoryWidgetClass);
    }

    // 생성된 인벤토리 위젯을 화면에 표시하고 현재 아이템 목록으로 갱신
    if (InventoryWidget)
    {
        InventoryWidget->AddToViewport();
        InventoryWidget->RefreshInventory(InventoryItems, MaxItemCount, SelectedSlotIndex);
    }
}

// 인벤토리 위젯을 화면에서 제거하는 함수
void UFZFInventoryComponent::HideInventory()
{
    // 생성된 인벤토리 위젯이 있으면 화면에서 제거
    if (InventoryWidget)
    {
        InventoryWidget->RemoveFromParent();
    }
}

// 선택한 슬롯 인덱스를 저장하는 함수
void UFZFInventoryComponent::SelectSlot(int32 InSlotIndex)
{
    // 선택한 슬롯 번호가 인벤토리 범위를 벗어나면 선택 해제
    if (InSlotIndex < 0 || InSlotIndex >= MaxItemCount)
    {
        SelectedSlotIndex = -1;
    }
    else
    {
        // 현재 선택 슬롯 인덱스 저장
        SelectedSlotIndex = InSlotIndex;
    }

    UE_LOG(LogTemp, Warning, TEXT("Selected Slot Index: %d"), SelectedSlotIndex);

    // 선택 상태 변경 후 UI 갱신
    if (InventoryWidget)
    {
        InventoryWidget->RefreshInventory(InventoryItems, MaxItemCount, SelectedSlotIndex);
    }

    // 선택된 슬롯에 맞춰 손에 들 아이템 갱신
    UpdateHeldItemBySelectedSlot();
}

UFZFItemData* UFZFInventoryComponent::GetSelectedItemData() const
{
    if (SelectedSlotIndex < 0 || SelectedSlotIndex > InventoryItems.Num() - 1)
        return nullptr;

    return InventoryItems[SelectedSlotIndex];
}

void UFZFInventoryComponent::RemoveSelectedItem()
{
    if (SelectedSlotIndex < 0 || SelectedSlotIndex >= MaxItemCount)
        return;

    InventoryItems[SelectedSlotIndex] = nullptr;

    if (InventoryWidget)
        InventoryWidget->RefreshInventory(InventoryItems, MaxItemCount, SelectedSlotIndex);

    // 삭제 후 현재 선택 슬롯 기준으로 손 아이템 다시 갱신
    UpdateHeldItemBySelectedSlot();
}

void UFZFInventoryComponent::UpdateHeldItemBySelectedSlot()
{
    // 이 컴포넌트를 가진 Owner에서 HeldItemComponent 찾기
    UFZFHeldItemComponent* HeldItemComponent =
        GetOwner()->FindComponentByClass<UFZFHeldItemComponent>();

    // 캐릭터에 HeldItemComponent가 없으면 처리 불가
    if (!HeldItemComponent)
    {
        return;
    }

    // 선택 인덱스가 InventoryItems 배열 안에 없으면 빈 슬롯으로 판단
    // 예: 슬롯은 5개인데 아이템은 2개만 있을 수 있음
    if (!InventoryItems.IsValidIndex(SelectedSlotIndex))
    {
        HeldItemComponent->ClearHeldItem();
        return;
    }

    // 선택된 슬롯의 ItemData 가져오기
    UFZFItemData* SelectedItemData = InventoryItems[SelectedSlotIndex];

    // 선택 슬롯에 아이템 데이터가 없으면 손 아이템 제거
    if (!SelectedItemData)
    {
        HeldItemComponent->ClearHeldItem();
        return;
    }

    // 선택된 아이템을 손에 들게 함
    HeldItemComponent->HoldItem(SelectedItemData);
}

void UFZFInventoryComponent::DropSelectedItem()
{
    // 선택된 슬롯 번호가 인벤토리 배열 범위 밖이면 버릴 아이템이 없음
    if (!InventoryItems.IsValidIndex(SelectedSlotIndex))
    {
        return;
    }

    // 선택된 슬롯의 ItemData 가져오기
    UFZFItemData* SelectedItemData = InventoryItems[SelectedSlotIndex];

    // 선택된 슬롯에 ItemData가 없으면 종료
    if (!SelectedItemData)
    {
        return;
    }


    // 이 InventoryComponent를 가지고 있는 Owner 가져오기
    AActor* OwnerActor = GetOwner();
    if (!OwnerActor)
    {
        return;
    }

    // 플레이어 위치 기준으로 아이템을 버릴 위치 계산
    FVector DropLocation =
        OwnerActor->GetActorLocation() +
        OwnerActor->GetActorForwardVector() * 150.0f;

    DropLocation.Z -= 50.0f;

    FRotator DropRotation = OwnerActor->GetActorRotation();
    FName ItemId = SelectedItemData->ItemId;
    int32 SlotIndexToClear = SelectedSlotIndex;

    // [중요] 클라이언트에서 배열을 직접 비우지 않고 서버에게 요청합니다.
    // 서버로부터 복제된 데이터가 도착하면 OnRep에 의해 UI가 갱신됩니다.

    // 서버에게 실제 스폰 및 서버 측 인벤토리 갱신 요청
    if (GetOwner()->HasAuthority())
    {
        ServerDropItem_Implementation(ItemId, DropLocation, DropRotation, SlotIndexToClear);
    }
    else
    {
        ServerDropItem(ItemId, DropLocation, DropRotation, SlotIndexToClear);
    }
}

void UFZFInventoryComponent::ServerDropItem_Implementation(FName InItemId, FVector SpawnLoc,
    FRotator SpawnRot, int32 SlotIndex)
{
    // 서버측 인벤토리 명시적 갱신
    if (InventoryItems.IsValidIndex(SlotIndex))
    {
        InventoryItems[SlotIndex] = nullptr;
        
        // 서버 측 UI 갱신 및 데이터 복제 트리거
        OnRep_InventoryItems();
        UpdateHeldItemBySelectedSlot();
    }

    AFZFCharacterPlayer* OwnerPlayer = Cast<AFZFCharacterPlayer>(GetOwner());
    if (OwnerPlayer)
    {
        AFZFPlayerController* PC = Cast<AFZFPlayerController>(OwnerPlayer->GetController());
        if (PC)
        {
            PC->RequestSpawnItem(InItemId, SpawnLoc, SpawnRot);
        }
    }
}
