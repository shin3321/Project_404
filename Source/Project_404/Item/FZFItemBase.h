// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FZFItemData.h"
#include "FZFItemBase.generated.h"

UCLASS()
class PROJECT_404_API AFZFItemBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFZFItemBase();

	UFZFItemData* GetItemData() const { return ItemData; }

protected:

	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

	void InitializeItem(UFZFItemData* InItemData);
	void ApplyItemData();
	void ApplyAutoFitMeshScale();

protected:

	UPROPERTY(VisibleAnywhere, Category = Item)
	TObjectPtr<class UBoxComponent> Trigger;

	// 상자를 보여주기 위한 메시 컴포넌트.
	UPROPERTY(VisibleAnywhere, Category = Item)
	TObjectPtr<class UStaticMeshComponent> Mesh;

	// 파티클 시스템 컴포넌트.
	UPROPERTY(VisibleAnywhere, Category = Item)
	TObjectPtr<class UParticleSystemComponent> Effect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	TObjectPtr<UFZFItemData> ItemData = nullptr;
};
