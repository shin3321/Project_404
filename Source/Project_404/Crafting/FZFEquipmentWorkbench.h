// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item/ItemTypes.h"
#include "Item/FZFItemBase.h"
#include "FZFEquipmentWorkbench.generated.h"

class UPrimitiveComponent;
class UBoxComponent;
class UStaticMeshComponent;
class USceneComponent;

class UFZFItemData;
class UFZFCraftPartItemData;
class UFZFEquipmentRecipeData;


// 제작대에서 상호작용 가능한 슬롯들.
UENUM(BlueprintType)
enum class EFZFWorkbenchSlot : uint8
{
	None UMETA(DisplayName = "None"),
	BaseSlot UMETA(DisplayName = "BasePart"),
	CoreSlot UMETA(DisplayName = "CorePart"),
	Crafting UMETA(DisplayName = "Crafting"),
	ResultSlot UMETA(DisplayName = "Crafting"),
};

UCLASS()
class PROJECT_404_API AFZFEquipmentWorkbench : public AActor
{
	GENERATED_BODY()

public:
	AFZFEquipmentWorkbench();

	bool TryInsertMaterialToSlot(EFZFWorkbenchSlot TargetSlot, UFZFItemData* ItemData);
	bool TryCraft();
	bool SpawnResultItem(UFZFEquipmentRecipeData* Recipe);

	UFUNCTION(BlueprintCallable, Category = "Workbench")
	bool InteractWithComponent(UPrimitiveComponent* HitComponent, UFZFItemData* HeldItemData, EFZFWorkbenchSlot& OutInteractedSlot);

	AFZFItemBase* GetSpawnedItem() const { return SpawnedItem; }

	void DestroySpawnedItem();

protected:
	virtual void BeginPlay() override;

private:
	UFZFEquipmentRecipeData* FindMatchedRecipe() const;
	EFZFWorkbenchSlot GetSlotFromHitComponent(UPrimitiveComponent* HitComponent) const;
	void UpdatePreviewMeshes();

protected:
	// BP에 만들어둔 컴포넌트를 이 변수에 할당하는 방식으로 처리하기 위해 Ref라는 네이밍을 추가적으로 붙임.
		
	// 충돌체 처리를 위한 BoxComponent들에 대한 변수 추가.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Workbench|Interaction")
	TObjectPtr<UBoxComponent> BasePartInteractionBoxRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Workbench|Interaction")
	TObjectPtr<UBoxComponent> CorePartInteractionBoxRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Workbench|Interaction")
	TObjectPtr<UBoxComponent> CombineButtonBoxRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Workbench|Interaction")
	TObjectPtr<UBoxComponent> ResultInteractionBoxRef;

	// 제작대에 아이템들이 위치되어야 할 소켓.
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Workbench|Socket")
	TObjectPtr<USceneComponent> BasePartSocketRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Workbench|Socket")
	TObjectPtr<USceneComponent> CorePartSocketRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Workbench|Socket")
	TObjectPtr<USceneComponent> ResultSocketRef;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Workbench|Recipe")
	TArray<TObjectPtr<UFZFEquipmentRecipeData>> Recipes;


private:
	// CraftMaterial 아이템을 든 상태에서 상호작용 시 보여줄 Mesh.
	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> BasePartMeshRef;

	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> CorePartMeshRef;

	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> ResultMeshRef;

private:
	UPROPERTY()
	TSubclassOf<AFZFItemBase> ResultItemActorClass;

	UPROPERTY()
	TObjectPtr<UFZFCraftPartItemData> CurrentBasePart;

	UPROPERTY()
	TObjectPtr<UFZFCraftPartItemData> CurrentCorePart;

private:
	UPROPERTY()
	TObjectPtr<AFZFItemBase> SpawnedItem;

};