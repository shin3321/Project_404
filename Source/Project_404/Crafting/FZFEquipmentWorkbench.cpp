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
		// 이미 서버라면 바로 실행
		Server_TryInsertMaterialToSlot_Implementation(TargetSlot, ItemData);
	}
	else
	{
		// 클라이언트라면 서버에 요청을 보냄
		Server_TryInsertMaterialToSlot(TargetSlot, ItemData);
	}
	return true;
}

void AFZFEquipmentWorkbench::Server_TryInsertMaterialToSlot_Implementation(EFZFWorkbenchSlot TargetSlot,
	UFZFItemData* ItemData)
{		
	if (ItemData == nullptr)
	{
		return ;
	}

	UFZFCraftPartItemData* CraftPartData = Cast<UFZFCraftPartItemData>(ItemData);
	if (CraftPartData == nullptr)
	{
		return ;
	}

	switch (TargetSlot)
	{
	case EFZFWorkbenchSlot::BaseSlot:
		{
			if (CraftPartData->SlotType != ECraftSlotType::BasePart)
			{
				return;
			}

			if (CurrentBasePart != nullptr)
			{
				return;
			}

			CurrentBasePart = CraftPartData;

			SetBasePartFrameRotating(true);

			break;
		}

	case EFZFWorkbenchSlot::CoreSlot:
		{
			if (CraftPartData->SlotType != ECraftSlotType::CorePart)
			{
				return;
			}

			if (CurrentCorePart != nullptr)
			{
				return;
			}

			CurrentCorePart = CraftPartData;

			SetCorePartFrameRotating(true);

			break;
		}
	default:
		return;
	}
	UpdatePreviewMeshes();
	
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
			PC->RequestSpawnItem(ItemId, SpawnLocation);
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