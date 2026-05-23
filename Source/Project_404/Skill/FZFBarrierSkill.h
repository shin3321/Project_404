// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Skill/FZFSkillBase.h"
#include "ActiveGameplayEffectHandle.h"
#include "FZFBarrierSkill.generated.h"


class USphereComponent;
class UStaticMeshComponent;
class UGameplayEffect;
class UAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class PROJECT_404_API AFZFBarrierSkill : public AFZFSkillBase
{
	GENERATED_BODY()
	
public:
	AFZFBarrierSkill();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Barrier")
	TObjectPtr<USphereComponent> BarrierCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Barrier")
	TObjectPtr<UStaticMeshComponent> BarrierMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Barrier")
	float BarrierRadius = 300.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barrier|GAS")
	TSubclassOf<UGameplayEffect> InvincibleEffectClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barrier|GAS")
	float EffectLevel = 1.0f;

private:
	UPROPERTY()
	TMap<TObjectPtr<UAbilitySystemComponent>, FActiveGameplayEffectHandle> AppliedEffectHandles;

private:
	void UpdateBarrierSize();

private:
	UFUNCTION()
	void OnBarrierBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	void OnBarrierEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

	void ApplyInvincibleEffect(AActor* TargetActor);
	void RemoveInvincibleEffect(AActor* TargetActor);
	void RemoveAllAppliedEffects();
};
