// Fill out your copyright notice in the Description page of Project Settings.

#include "Crafting/FZFEquipmentWorkbench.h"

#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PrimitiveComponent.h"

#include "Item/FZFItemData.h"
#include "Item/CraftMaterial/FZFCraftPartItemData.h"
#include "Item/Recipe/FZFEquipmentRecipeData.h"

#include "Character/Player/FZFCharacterPlayer.h"
#include "Inventory/FZFInventoryComponent.h"
#include "Utils/FZFUtils.h"

#include "Item/ItemTypes.h"
#include"Character/Player/FZFPlayerController.h"
#include "Net/UnrealNetwork.h"


AFZFEquipmentWorkbench::AFZFEquipmentWorkbench()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	SetReplicates(true);

	ResultItemActorClass = AFZFItemBase::StaticClass();
}

void AFZFEquipmentWorkbench::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AFZFEquipmentWorkbench, CurrentBasePart);
	DOREPLIFETIME(AFZFEquipmentWorkbench, CurrentCorePart);
}

void AFZFEquipmentWorkbench::DestroySpawnedItem()
{
	SpawnedItem->Destroy();
	SpawnedItem = nullptr;

	UpdatePreviewMeshes();
}

void AFZFEquipmentWorkbench::BeginPlay()
{
	Super::BeginPlay();

	TArray<UActorComponent*> Components;
	GetComponents(Components);

	BasePartInteractionBoxRef = FZFFindComponentByName<UBoxComponent>(this, TEXT("BasePartInteractionBox"));
	CorePartInteractionBoxRef = FZFFindComponentByName<UBoxComponent>(this, TEXT("CorePartInteractionBox"));
	CombineButtonBoxRef = FZFFindComponentByName<UBoxComponent>(this, TEXT("CombineButtonBox"));
	ResultInteractionBoxRef = FZFFindComponentByName<UBoxComponent>(this, TEXT("ResultInteractionBox"));

	BasePartMeshRef = FZFFindComponentByName<UStaticMeshComponent>(this, TEXT("BasePartPreviewMesh"));
	CorePartMeshRef = FZFFindComponentByName<UStaticMeshComponent>(this, TEXT("CorePartPreviewMesh"));
	ResultMeshRef = FZFFindComponentByName<UStaticMeshComponent>(this, TEXT("ResultPreviewMesh"));
	CorePartFrameMeshRef = FZFFindComponentByName<UStaticMeshComponent>(this, TEXT("CorePartFrame"));
	BasePartFrameMeshRef = FZFFindComponentByName<UStaticMeshComponent>(this, TEXT("BasePartFrame"));

	BasePartGhostMeshRef = FZFFindComponentByName<UStaticMeshComponent>(this, TEXT("BasePartGhostMesh"));
	CorePartGhostMeshRef = FZFFindComponentByName<UStaticMeshComponent>(this, TEXT("CorePartGhostMesh"));

	auto SetupInteractionBox = [](UBoxComponent* Box)
	{
		if (Box == nullptr)
		{
			return;
		}

		Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		Box->SetCollisionResponseToAllChannels(ECR_Ignore);
		Box->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	};

	SetupInteractionBox(BasePartInteractionBoxRef);
	SetupInteractionBox(CorePartInteractionBoxRef);
	SetupInteractionBox(CombineButtonBoxRef);
	SetupInteractionBox(ResultInteractionBoxRef);

	UpdatePreviewMeshes();
}

void AFZFEquipmentWorkbench::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bRotateBasePartFrame && IsValid(BasePartFrameMeshRef))
	{
		const float DeltaYaw = BasePartFrameRotateSpeed * DeltaTime;

		BasePartFrameMeshRef->AddWorldRotation(
			FRotator(0.0f, DeltaYaw, 0.0f)
		);
	}

	if (bRotateCorePartFrame && IsValid(CorePartFrameMeshRef))
	{
		const float DeltaYaw = CorePartFrameRotateSpeed * DeltaTime;

		CorePartFrameMeshRef->AddWorldRotation(
			FRotator(0.0f, DeltaYaw, 0.0f)
		);
	}
}

bool AFZFEquipmentWorkbench::CanInsertMaterialToSlot(
	EFZFWorkbenchSlot TargetSlot,
	UFZFItemData* ItemData
) const
{
	if (ItemData == nullptr)
	{
		return false;
	}

	UFZFCraftPartItemData* CraftPartData = Cast<UFZFCraftPartItemData>(ItemData);
	if (CraftPartData == nullptr)
	{
		return false;
	}

	switch (TargetSlot)
	{
	case EFZFWorkbenchSlot::BaseSlot:
	{
		if (CraftPartData->SlotType != ECraftSlotType::BasePart)
		{
			return false;
		}

		if (CurrentBasePart != nullptr)
		{
			return false;
		}

		return true;
	}

	case EFZFWorkbenchSlot::CoreSlot:
	{
		if (CraftPartData->SlotType != ECraftSlotType::CorePart)
		{
			return false;
		}

		if (CurrentCorePart != nullptr)
		{
			return false;
		}

		return true;
	}

	default:
		return false;
	}
}

void AFZFEquipmentWorkbench::SetBasePartFrameRotating(bool bShouldRotate)
{
	bRotateBasePartFrame = bShouldRotate;
}

void AFZFEquipmentWorkbench::SetCorePartFrameRotating(bool bShouldRotate)
{
	bRotateCorePartFrame = bShouldRotate;
}

EFZFWorkbenchSlot AFZFEquipmentWorkbench::GetSlotFromHitComponent(UPrimitiveComponent* HitComponent) const
{
	if (HitComponent == nullptr)
		return EFZFWorkbenchSlot::None;

	if (HitComponent == BasePartInteractionBoxRef)
		return EFZFWorkbenchSlot::BaseSlot;

	if (HitComponent == CorePartInteractionBoxRef)
		return EFZFWorkbenchSlot::CoreSlot;

	if (HitComponent == CombineButtonBoxRef)
		return EFZFWorkbenchSlot::Crafting;

	if (HitComponent == ResultInteractionBoxRef)
		return EFZFWorkbenchSlot::ResultSlot;

	return EFZFWorkbenchSlot::None;
}

bool AFZFEquipmentWorkbench::TryInsertMaterialToSlot(EFZFWorkbenchSlot TargetSlot, UFZFItemData* ItemData)
{
	if (HasAuthority())
	{
		return InsertMaterialToSlot_Internal(TargetSlot, ItemData);
	}

	// 클라이언트에서도 미리 실패할 수 있는 조건은 검사 가능
	if (!CanInsertMaterialToSlot(TargetSlot, ItemData))
	{
		return false;
	}

	// 서버에 요청
	Server_TryInsertMaterialToSlot(TargetSlot, ItemData);
	return true;
}

bool AFZFEquipmentWorkbench::InsertMaterialToSlot_Internal(
	EFZFWorkbenchSlot TargetSlot,
	UFZFItemData* ItemData
)
{
	if (!CanInsertMaterialToSlot(TargetSlot, ItemData))
	{
		return false;
	}

	UFZFCraftPartItemData* CraftPartData = Cast<UFZFCraftPartItemData>(ItemData);
	if (CraftPartData == nullptr)
	{
		return false;
	}

	switch (TargetSlot)
	{
	case EFZFWorkbenchSlot::BaseSlot:
	{
		CurrentBasePart = CraftPartData;
		SetBasePartFrameRotating(true);
		break;
	}

	case EFZFWorkbenchSlot::CoreSlot:
	{
		CurrentCorePart = CraftPartData;
		SetCorePartFrameRotating(true);
		break;
	}

	default:
		return false;
	}

	UpdatePreviewMeshes();

	return true;
}

void AFZFEquipmentWorkbench::Server_TryInsertMaterialToSlot_Implementation(
	EFZFWorkbenchSlot TargetSlot,
	UFZFItemData* ItemData
)
{
	const bool bInserted = InsertMaterialToSlot_Internal(TargetSlot, ItemData);

	if (!bInserted)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to insert material to workbench slot."));
	}
}

void AFZFEquipmentWorkbench::OnRep_WorkbenchParts()
{	
	UpdatePreviewMeshes();
}

bool AFZFEquipmentWorkbench::TryCraft()
{
	if (CurrentBasePart == nullptr || CurrentCorePart == nullptr)
	{
		return false;
	}

	UFZFEquipmentRecipeData* MatchedRecipe = FindMatchedRecipe();
	if (MatchedRecipe == nullptr)
	{
		return false;
	}

	if (!SpawnResultItem(MatchedRecipe))
	{
		return false;
	}

	CurrentBasePart = nullptr;
	CurrentCorePart = nullptr;

	UpdatePreviewMeshes();
	ResetWorkbench();

	return true;
}

bool AFZFEquipmentWorkbench::SpawnResultItem(UFZFEquipmentRecipeData* Recipe)
{
	if (Recipe == nullptr)
		return false;

	if (ResultItemActorClass == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("ResultItemActorClass is not assigned."));
		return false;
	}

	FName ItemId = Recipe->ResultItem->GetItemId();

	UWorld* World = GetWorld();
	if (World == nullptr)
		return false;

	const FVector SpawnLocation = ResultMeshRef->GetComponentLocation();
	const FRotator SpawnRotation = ResultMeshRef->GetComponentRotation();
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	if (TargetInteractor)
	{
		AFZFPlayerController* PC = Cast<AFZFPlayerController>(TargetInteractor->GetController());
		if (PC)
		{
			PC->RequestSpawnItem(ItemId, SpawnLocation, SpawnRotation);
			return true;
		}
	}
	return false;
}

// Recipe데이터에 일치하는 것이 존재한다면 해당 Recipe를 반환하는 함수.
UFZFEquipmentRecipeData* AFZFEquipmentWorkbench::FindMatchedRecipe() const
{
	if (CurrentBasePart == nullptr || CurrentCorePart == nullptr)
	{
		return nullptr;
	}

	for (UFZFEquipmentRecipeData* Recipe : Recipes)
	{
		if (Recipe == nullptr)
		{
			continue;
		}

		if (Recipe->BasePart == CurrentBasePart &&
			Recipe->CorePart == CurrentCorePart)
		{
			return Recipe;
		}
	}

	return nullptr;
}

// 미리보기 업데이트.
void AFZFEquipmentWorkbench::UpdatePreviewMeshes()
{
	if (BasePartMeshRef)
	{
		if (CurrentBasePart && CurrentBasePart->Mesh)
		{
			BasePartMeshRef->SetStaticMesh(CurrentBasePart->Mesh);
			BasePartMeshRef->SetVisibility(true);
		}
		else
		{
			BasePartMeshRef->SetStaticMesh(nullptr);
			BasePartMeshRef->SetVisibility(false);
		}
	}

	if (CorePartMeshRef)
	{
		if (CurrentCorePart && CurrentCorePart->Mesh)
		{
			CorePartMeshRef->SetStaticMesh(CurrentCorePart->Mesh);
			CorePartMeshRef->SetVisibility(true);
		}
		else
		{
			CorePartMeshRef->SetStaticMesh(nullptr);
			CorePartMeshRef->SetVisibility(false);
		}
	}

	if (ResultMeshRef)
	{
		ResultMeshRef->SetVisibility(false);
	}

	UpdateGhostPreviewMeshes();
}

void AFZFEquipmentWorkbench::Interact(AFZFCharacterPlayer* Interactor, UPrimitiveComponent* HitComponent)
{
	if (!IsValid(Interactor))
		return;
	TargetInteractor = Interactor;
	UFZFInventoryComponent* Inventory = Interactor->GetInventoryComponent();

	UFZFItemData* HeldItemData = Inventory->GetSelectedItemData();

	// 충돌한 Slot을 가져와서 switch문의 케이스별로 각 슬롯들이 시도해야 하는 동작을 수행한다.
	EFZFWorkbenchSlot HitSlot = GetSlotFromHitComponent(HitComponent);
	switch (HitSlot)
	{
	case EFZFWorkbenchSlot::BaseSlot:
	case EFZFWorkbenchSlot::CoreSlot:
		{
			// 1. 슬롯에 이미 부품이 있으면 회수 시도
			if (GetMaterialInSlot(HitSlot) != nullptr)
			{
				if (TryTakeBackMaterialFromSlot(HitSlot, Interactor) == false)
				{
					UE_LOG(LogTemp, Log, TEXT("Failed to take back material from workbench slot."));
				}

				return;
			}

			// 2. 슬롯이 비어있는데 손에 든 아이템이 없으면 아무것도 안 함
			if (HeldItemData == nullptr)
			{
				return;
			}

			// 3. 슬롯이 비어있고 손에 든 아이템이 있으면 삽입 시도
			if (TryInsertMaterialToSlot(HitSlot, HeldItemData) == false)
			{
				UE_LOG(LogTemp, Log, TEXT("Failed to insert material into workbench slot."));
				return;
			}

			Inventory->RemoveSelectedItem();
			break;
		}
	case EFZFWorkbenchSlot::Crafting:
		if (TryCraft() == false)
		{
			UE_LOG(LogTemp, Log, TEXT("Failed to craft item at workbench"));
			return;
		}
		break;

	default:
		break;
	}
}

FText AFZFEquipmentWorkbench::GetInteractableName(UPrimitiveComponent* HitComponent) const
{
	const EFZFWorkbenchSlot HitSlot = GetSlotFromHitComponent(HitComponent);

	switch (HitSlot)
	{
	case EFZFWorkbenchSlot::BaseSlot:
		return FText::FromString(TEXT("Base Part"));

	case EFZFWorkbenchSlot::CoreSlot:
		return FText::FromString(TEXT("Core Part"));

	case EFZFWorkbenchSlot::Crafting:
		return FText::FromString(TEXT("Craft"));

	case EFZFWorkbenchSlot::ResultSlot:
		return FText::FromString(TEXT("GetItem"));

	default:
		return FText::GetEmpty();
	}
}

void AFZFEquipmentWorkbench::ResetWorkbench()
{
	CurrentBasePart = nullptr;
	CurrentCorePart = nullptr;

	UpdatePreviewMeshes();

	SetBasePartFrameRotating(false);
	SetCorePartFrameRotating(false);
}

UFZFCraftPartItemData* AFZFEquipmentWorkbench::GetMaterialInSlot(EFZFWorkbenchSlot TargetSlot) const
{
	switch (TargetSlot)
	{
	case EFZFWorkbenchSlot::BaseSlot:
		return CurrentBasePart;

	case EFZFWorkbenchSlot::CoreSlot:
		return CurrentCorePart;

	default:
		return nullptr;
	}
}

void AFZFEquipmentWorkbench::ClearMaterialSlot_Internal(EFZFWorkbenchSlot TargetSlot)
{
	switch (TargetSlot)
	{
	case EFZFWorkbenchSlot::BaseSlot:
		CurrentBasePart = nullptr;
		SetBasePartFrameRotating(false);
		break;

	case EFZFWorkbenchSlot::CoreSlot:
		CurrentCorePart = nullptr;
		SetCorePartFrameRotating(false);
		break;

	default:
		break;
	}

	UpdatePreviewMeshes();
}

bool AFZFEquipmentWorkbench::TryTakeBackMaterialFromSlot(
	EFZFWorkbenchSlot TargetSlot,
	AFZFCharacterPlayer* Interactor
)
{
	if (HasAuthority())
	{
		return TakeBackMaterialFromSlot_Internal(TargetSlot, Interactor);
	}

	Server_TryTakeBackMaterialFromSlot(TargetSlot, Interactor);
	return true;
}

void AFZFEquipmentWorkbench::Server_TryTakeBackMaterialFromSlot_Implementation(
	EFZFWorkbenchSlot TargetSlot,
	AFZFCharacterPlayer* Interactor
)
{
	const bool bTakeBackSucceeded = TakeBackMaterialFromSlot_Internal(TargetSlot, Interactor);

	if (!bTakeBackSucceeded)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to take back material from workbench slot."));
	}
}

bool AFZFEquipmentWorkbench::TakeBackMaterialFromSlot_Internal(EFZFWorkbenchSlot TargetSlot, AFZFCharacterPlayer* Interactor)
{
	if (!HasAuthority())
	{
		return false;
	}

	if (!IsValid(Interactor))
	{
		return false;
	}

	UFZFInventoryComponent* Inventory = Interactor->GetInventoryComponent();
	if (!IsValid(Inventory))
	{
		return false;
	}

	UFZFCraftPartItemData* SlotItemData = GetMaterialInSlot(TargetSlot);
	if (!IsValid(SlotItemData))
	{
		return false;
	}

	// 인벤토리에 빈 칸이 있을 때만 AddItem이 true를 반환한다고 가정.
	// 실패하면 제작대 슬롯은 비우면 안됨.
	const bool bAddedToInventory = Inventory->AddItem(SlotItemData);
	if (!bAddedToInventory)
	{
		UE_LOG(LogTemp, Warning, TEXT("Inventory is full. Cannot take back material."));
		return false;
	}

	ClearMaterialSlot_Internal(TargetSlot);

	return true;
}

UFZFCraftPartItemData* AFZFEquipmentWorkbench::FindRequiredCorePartByBasePart(
	UFZFCraftPartItemData* BasePart
) const
{
	if (BasePart == nullptr)
	{
		return nullptr;
	}

	for (UFZFEquipmentRecipeData* Recipe : Recipes)
	{
		if (Recipe == nullptr)
		{
			continue;
		}

		if (Recipe->BasePart == BasePart)
		{
			return Recipe->CorePart;
		}
	}

	return nullptr;
}

UFZFCraftPartItemData* AFZFEquipmentWorkbench::FindRequiredBasePartByCorePart(
	UFZFCraftPartItemData* CorePart
) const
{
	if (CorePart == nullptr)
	{
		return nullptr;
	}

	for (UFZFEquipmentRecipeData* Recipe : Recipes)
	{
		if (Recipe == nullptr)
		{
			continue;
		}

		if (Recipe->CorePart == CorePart)
		{
			return Recipe->BasePart;
		}
	}

	return nullptr;
}

void AFZFEquipmentWorkbench::UpdateGhostPreviewMeshes()
{
	// 일단 Ghost 전부 초기화
	if (BasePartGhostMeshRef)
	{
		BasePartGhostMeshRef->SetStaticMesh(nullptr);
		BasePartGhostMeshRef->SetVisibility(false);
	}

	if (CorePartGhostMeshRef)
	{
		CorePartGhostMeshRef->SetStaticMesh(nullptr);
		CorePartGhostMeshRef->SetVisibility(false);
	}

	// 아무것도 없으면 Ghost 없음
	if (CurrentBasePart == nullptr && CurrentCorePart == nullptr)
	{
		return;
	}

	// 현재 조합이 완성된 레시피라면 Ghost 없음
	if (CurrentBasePart != nullptr && CurrentCorePart != nullptr)
	{
		if (FindMatchedRecipe() != nullptr)
		{
			return;
		}

		// 둘 다 들어갔지만 조합이 틀린 상태.
		// 이 경우 어떤 쪽을 기준으로 힌트를 줄지 정해야 한다.
		//
		// 추천:
		// BasePart를 기준으로 필요한 CorePart를 보여준다.
		// 즉, "이 BasePart에는 이 CorePart가 필요하다"를 알려준다.
		UFZFCraftPartItemData* RequiredCorePart =
			FindRequiredCorePartByBasePart(CurrentBasePart);

		if (RequiredCorePart && RequiredCorePart->Mesh && CorePartGhostMeshRef)
		{
			CorePartGhostMeshRef->SetStaticMesh(RequiredCorePart->Mesh);

			if (GhostMaterial)
			{
				CorePartGhostMeshRef->SetMaterial(0, GhostMaterial);
			}

			CorePartGhostMeshRef->SetVisibility(true);
		}

		return;
	}

	// BasePart만 들어간 상태
	// 필요한 CorePart를 Ghost로 표시
	if (CurrentBasePart != nullptr && CurrentCorePart == nullptr)
	{
		UFZFCraftPartItemData* RequiredCorePart =
			FindRequiredCorePartByBasePart(CurrentBasePart);

		if (RequiredCorePart && RequiredCorePart->Mesh && CorePartGhostMeshRef)
		{
			CorePartGhostMeshRef->SetStaticMesh(RequiredCorePart->Mesh);

			if (GhostMaterial)
			{
				CorePartGhostMeshRef->SetMaterial(0, GhostMaterial);
			}

			CorePartGhostMeshRef->SetVisibility(true);
		}

		return;
	}

	// CorePart만 들어간 상태
	// 필요한 BasePart를 Ghost로 표시
	if (CurrentBasePart == nullptr && CurrentCorePart != nullptr)
	{
		UFZFCraftPartItemData* RequiredBasePart =
			FindRequiredBasePartByCorePart(CurrentCorePart);

		if (RequiredBasePart && RequiredBasePart->Mesh && BasePartGhostMeshRef)
		{
			BasePartGhostMeshRef->SetStaticMesh(RequiredBasePart->Mesh);

			if (GhostMaterial)
			{
				BasePartGhostMeshRef->SetMaterial(0, GhostMaterial);
			}

			BasePartGhostMeshRef->SetVisibility(true);
		}

		return;
	}
}