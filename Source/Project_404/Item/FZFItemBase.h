// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/FZFInteractableInterface.h"
#include "FZFItemData.h"
#include "FZFItemBase.generated.h"

class AFZFCharacterPlayer;
class UPrimitiveComponent;
class UNiagaraComponent;
class UNiagaraSystem;

UCLASS()
class PROJECT_404_API AFZFItemBase : public AActor, public IFZFInteractableInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFZFItemBase();

	UFZFItemData* GetItemData() const { return ItemData; }
	void InitializeItem(UFZFItemData* InItemData);

	UFUNCTION()
	void OnRep_ItemData();

	void Interact(AFZFCharacterPlayer* Interactor, UPrimitiveComponent* HitComponent) override;
	FText GetInteractableName(UPrimitiveComponent* HitComponent) const override;

	UFUNCTION(BlueprintCallable, Category = "Item|Placement")
	void PlaceOnGround();

	UFUNCTION(BlueprintCallable, Category = "Item|Placement")
	void ApplyGroundRotation();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:

	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

	void ApplyItemData();
	void ApplyAutoFitMeshScale();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|Effect")
	TObjectPtr<UNiagaraComponent> WeaponParticle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|Effect")
	TObjectPtr<UNiagaraComponent> MaterialParticle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|Effect")
	TObjectPtr<UNiagaraComponent> RobotPartParticle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Effect")
	TObjectPtr<UNiagaraSystem> WeaponParticleSystem;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Effect")
	TObjectPtr<UNiagaraSystem> MaterialParticleSystem;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Effect")
	TObjectPtr<UNiagaraSystem> RobotPartParticleSystem;
	
protected:

	UPROPERTY(VisibleAnywhere, Category = Item)
	TObjectPtr<class UBoxComponent> Trigger;

	// 상자를 보여주기 위한 메시 컴포넌트.
	UPROPERTY(VisibleAnywhere, Category = Item)
	TObjectPtr<class UStaticMeshComponent> Mesh;

	// 파티클 시스템 컴포넌트.
	UPROPERTY(VisibleAnywhere, Category = Item)
	TObjectPtr<class UParticleSystemComponent> Effect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_ItemData, Category = "Item")
	TObjectPtr<UFZFItemData> ItemData = nullptr;

protected:
	void UpdateItemParticle();

	void DeactivateAllItemParticles();
};
