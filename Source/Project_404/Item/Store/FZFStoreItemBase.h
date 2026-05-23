// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/FZFItemBase.h"
#include "FZFStoreItemBase.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_404_API AFZFStoreItemBase : public AFZFItemBase
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFZFStoreItemBase();
	UFZFItemData* GetItemData() const { return ItemData; }

	void Interact(AFZFCharacterPlayer* Interactor, UPrimitiveComponent* HitComponent) override;
	int32 GetCost() const{ return Cost;}
	
protected:
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;
	
private:
	int32 Cost;
};
