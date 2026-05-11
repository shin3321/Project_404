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


namespace
{
	template <typename T>
	T* FindComponentByName(AActor* Owner, const FString& ComponentName)
	{
		if (Owner == nullptr)
		{
			return nullptr;
		}

		TArray<T*> Components;
		Owner->GetComponents<T>(Components);

		for (T* Component : Components)
		{
			if (Component && Component->GetName() == ComponentName)
			{
				return Component;
			}
		}

		return nullptr;
	}
}

AFZFEquipmentWorkbench::AFZFEquipmentWorkbench()
{
	PrimaryActorTick.bCanEverTick = false;

	ResultItemActorClass = AFZFItemBase::StaticClass();
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

	BasePartInteractionBoxRef = FindComponentByName<UBoxComponent>(this, TEXT("BasePartInteractionBox"));
	CorePartInteractionBoxRef = FindComponentByName<UBoxComponent>(this, TEXT("CorePartInteractionBox"));
	CombineButtonBoxRef = FindComponentByName<UBoxComponent>(this, TEXT("CombineButtonBox"));
	ResultInteractionBoxRef = FindComponentByName<UBoxComponent>(this, TEXT("ResultInteractionBox"));

	BasePartMeshRef = FindComponentByName<UStaticMeshComponent>(this, TEXT("BasePartPreviewMesh"));
	CorePartMeshRef = FindComponentByName<UStaticMeshComponent>(this, TEXT("CorePartPreviewMesh"));
	ResultMeshRef = FindComponentByName<UStaticMeshComponent>(this, TEXT("ResultPreviewMesh"));

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

		CurrentBasePart = CraftPartData;
		break;
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

		CurrentCorePart = CraftPartData;
		break;
	}

	default:
		return false;
	}

	UpdatePreviewMeshes();

	return true;
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

	UWorld* World = GetWorld();
	if (World == nullptr)
		return false;

	const FVector SpawnLocation = ResultMeshRef->GetComponentLocation();
	const FRotator SpawnRotation = ResultMeshRef->GetComponentRotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;

	SpawnedItem = World->SpawnActor<AFZFItemBase>(
		ResultItemActorClass,
		SpawnLocation,
		SpawnRotation,
		SpawnParams
	);

	if (SpawnedItem == nullptr)
		return false;

	SpawnedItem->InitializeItem(Recipe->ResultItem);

	return true;
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
}

void AFZFEquipmentWorkbench::Interact(AFZFCharacterPlayer* Interactor, UPrimitiveComponent* HitComponent)
{
	if (!IsValid(Interactor))
		return;

	UFZFInventoryComponent* Inventory = Interactor->GetInventoryComponent();

	// [Youngwoo Test] : InventoryItems[0]를 -> 선택한 아이템으로 수정해줘야됨.
	UFZFItemData* HeldItemData = nullptr;
	if (Inventory->InventoryItems.Num() > 0)
		HeldItemData = Inventory->InventoryItems[0];

	// 충돌한 Slot을 가져와서 switch문의 케이스별로 각 슬롯들이 시도해야 하는 동작을 수행한다.
	EFZFWorkbenchSlot HitSlot = GetSlotFromHitComponent(HitComponent);
	switch (HitSlot)
	{
	case EFZFWorkbenchSlot::BaseSlot:
	case EFZFWorkbenchSlot::CoreSlot:
	{
		if (TryInsertMaterialToSlot(HitSlot, HeldItemData) == false)
		{
			UE_LOG(LogTemp, Log, TEXT("Failed to insert material into workbench slot."));
			return;
		}

		Inventory->RemoveSelectedItem(HeldItemData);
		break;
	}
	case EFZFWorkbenchSlot::Crafting:
		if (TryCraft() == false)
		{
			UE_LOG(LogTemp, Log, TEXT("Failed to craft item at workbench"));
			return;
		}
		break;

	case EFZFWorkbenchSlot::ResultSlot:
		if (IsValid(SpawnedItem) == false)
			return;

		if (Inventory->AddItem(SpawnedItem->GetItemData()))
			DestroySpawnedItem();

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
		return FText::FromString(TEXT("Insert Base Part"));

	case EFZFWorkbenchSlot::CoreSlot:
		return FText::FromString(TEXT("Insert Core Part"));

	case EFZFWorkbenchSlot::Crafting:
		return FText::FromString(TEXT("Craft Equipment"));

	case EFZFWorkbenchSlot::ResultSlot:
		return FText::FromString(TEXT("Take Result Item"));

	default:
		return FText::GetEmpty();
	}
}
