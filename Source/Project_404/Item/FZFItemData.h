// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ItemTypes.h"
#include "Engine/Texture2D.h"
#include "FZFItemData.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_404_API UFZFItemData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    FName ItemId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    FText ItemName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    TObjectPtr<class UStaticMesh> Mesh = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World")
    FVector MeshScale = FVector(1.0f, 1.0f, 1.0f);

    // 메쉬 사이즈를 Trigger Collision의 사이즈에 맞게 자동으로 조정해주는 변수.
    // 사용 이유 : 애셋들의 모든 사이즈를 Trigger Collision의 사이즈에 맞게 일일이 수동으로 해주기보다는 자동으로 맞춰주도록 해주기 위해.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World")
    bool bAutoFitMeshToTrigger = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    TObjectPtr<UTexture2D> Icon = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    EItemType ItemType = EItemType::Equipment;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    int32 MaxStackCount = 1;
};
