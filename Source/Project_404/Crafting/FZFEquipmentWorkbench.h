// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item/ItemTypes.h"
#include "Item/FZFItemBase.h"
#include "Interface/FZFInteractableInterface.h"
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
class PROJECT_404_API AFZFEquipmentWorkbench : public AActor, public IFZFInteractableInterface
{
	GENERATED_BODY()

public:
	AFZFEquipmentWorkbench();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	bool SpawnResultItem(UFZFEquipmentRecipeData* Recipe);
	AFZFItemBase* GetSpawnedItem() const { return SpawnedItem; }
	void DestroySpawnedItem();

	// IFZFInteractableInterface의 순수 가상함수 구현.
	void Interact(AFZFCharacterPlayer* Interactor, UPrimitiveComponent* HitComponent) override;
	FText GetInteractableName(UPrimitiveComponent* HitComponent) const override;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

private:
	UFZFEquipmentRecipeData* FindMatchedRecipe() const;

	EFZFWorkbenchSlot GetSlotFromHitComponent(UPrimitiveComponent* HitComponent) const;

	// 재료를 슬롯에 넣기를 시도하는 함수.
	bool TryInsertMaterialToSlot(EFZFWorkbenchSlot TargetSlot, UFZFItemData* ItemData);

	// 서버에게 실행해 달라고 요청하는 함수 (Server RPC)
	UFUNCTION(Server, Reliable)
	void Server_TryInsertMaterialToSlot(EFZFWorkbenchSlot TargetSlot, UFZFItemData* ItemData);

	bool InsertMaterialToSlot_Internal(EFZFWorkbenchSlot TargetSlot, UFZFItemData* ItemData);
	
	// 제작 버튼을 누르는 함수.
	bool TryCraft();

	void UpdatePreviewMeshes();

	bool CanInsertMaterialToSlot(EFZFWorkbenchSlot TargetSlot, UFZFItemData* ItemData) const;

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
	
public:
	UPROPERTY(ReplicatedUsing=OnRep_WorkbenchParts, Transient, VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UFZFCraftPartItemData> CurrentBasePart;

	UPROPERTY(ReplicatedUsing=OnRep_WorkbenchParts, Transient, VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UFZFCraftPartItemData> CurrentCorePart;
	
	// 클라이언트에서 변수 동기화 시 호출될 함수
	UFUNCTION()
	void OnRep_WorkbenchParts();
private:
	UPROPERTY()
	TObjectPtr<AFZFItemBase> SpawnedItem;
	
	UPROPERTY(Replicated)
	AFZFCharacterPlayer* TargetInteractor;

// 제작대 효과.
protected:
	// Blueprint 안에 있는 CorePartFrame Mesh를 연결
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Workbench|Frame")
	TObjectPtr<UStaticMeshComponent> CorePartFrameMeshRef;

	// Blueprint 안에 있는 BasePartFrame Mesh를 연결
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Workbench|Frame")
	TObjectPtr<UStaticMeshComponent> BasePartFrameMeshRef;

	// BasePartFrame 회전 여부
	UPROPERTY(ReplicatedUsing=OnRep_WorkbenchParts, VisibleAnywhere, BlueprintReadOnly, Category = "Workbench|Frame")
	bool bRotateBasePartFrame = false;

	// CorePartFrame 회전 여부
	UPROPERTY(ReplicatedUsing=OnRep_WorkbenchParts, VisibleAnywhere, BlueprintReadOnly, Category = "Workbench|Frame")
	bool bRotateCorePartFrame = false;

	// BasePartFrame Z축 회전 속도, 초당 도 단위
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Workbench|Frame")
	float BasePartFrameRotateSpeed = 90.0f;

	// BasePartFrame Z축 회전 속도, 초당 도 단위
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Workbench|Frame")
	float CorePartFrameRotateSpeed = 120.0f;

// 회수 관련 함수.
protected:

	bool TryTakeBackMaterialFromSlot(EFZFWorkbenchSlot TargetSlot, AFZFCharacterPlayer* Interactor);

	bool TakeBackMaterialFromSlot_Internal(EFZFWorkbenchSlot TargetSlot, AFZFCharacterPlayer* Interactor);

	UFZFCraftPartItemData* GetMaterialInSlot(EFZFWorkbenchSlot TargetSlot) const;

	void ClearMaterialSlot_Internal(EFZFWorkbenchSlot TargetSlot);

	UFUNCTION(Server, Reliable)
	void Server_TryTakeBackMaterialFromSlot(EFZFWorkbenchSlot TargetSlot, AFZFCharacterPlayer* Interactor);

protected:

	void SetBasePartFrameRotating(bool bShouldRotate);
	void SetCorePartFrameRotating(bool bShouldRotate);
	void ResetWorkbench();

// 빨간 재질의 아이템 보여주기.
protected:

	// Ghost Preview Mesh
	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> BasePartGhostMeshRef;

	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> CorePartGhostMeshRef;

	// 빨간 Ghost 재질
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Workbench|Ghost")
	TObjectPtr<UMaterialInterface> GhostMaterial;

	// Ghost Preview 갱신
	void UpdateGhostPreviewMeshes();

	// 레시피 기준으로 필요한 반대 부품 찾기
	UFZFCraftPartItemData* FindRequiredCorePartByBasePart(UFZFCraftPartItemData* BasePart) const;
	UFZFCraftPartItemData* FindRequiredBasePartByCorePart(UFZFCraftPartItemData* CorePart) const;
};
