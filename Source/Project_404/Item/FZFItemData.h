// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ItemTypes.h"
#include "Engine/Texture2D.h"
#include "GameplayTagContainer.h"
#include "FZFItemData.generated.h"

class UNiagaraSystem;

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

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World Placement")
    FRotator GroundRotation = FRotator::ZeroRotator;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World Placement")
    bool bRandomGroundYaw = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    TObjectPtr<UTexture2D> Icon = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    EItemType ItemType = EItemType::Equipment;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    int32 MaxStackCount = 1;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
    TObjectPtr<class UFZFItemAnimSetData> AnimSet;

    // 이 무기를 들었을 때 부여할 Ability
    UPROPERTY(EditAnywhere, Category = "GAS")
    TSubclassOf<class UGameplayAbility> ItemAbilityClass;

    UPROPERTY(EditAnywhere, Category = "GAS")
    FGameplayTag ItemAbilityTag;

    // 이 무기가 타겟에게 적용할 효과 (데미지, 기절 등)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
    TArray<TSubclassOf<class UGameplayEffect>> AllowedEffectClasses;

    // 메쉬 사이즈를 Trigger Collision의 사이즈에 맞게 자동으로 조정해주는 변수.
    // 사용 이유 : 애셋들의 모든 사이즈를 Trigger Collision의 사이즈에 맞게 일일이 수동으로 해주기보다는 자동으로 맞춰주도록 해주기 위해.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World")
    bool bAutoFitMeshToTrigger = true;

    // 이 체크박스를 켜면 소켓 위치에서 판정을 시작
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Socket")
    bool bUseSocketTargeting = true;

    // 판정의 시작점으로 사용할 소켓 이름 (예: Hand_R_Socket, Muzzle_Socket)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Socket")
    FName StartSocketName;
	
	// 스폰 클래스를 결정하는 변수
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Config")
	TSubclassOf<AActor> ItemActorClass;

    // 무기 발사 시 사용할 나이아가라 레이저 이펙트
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual")
    TObjectPtr<UNiagaraSystem> WeaponLaserEffect;
};
