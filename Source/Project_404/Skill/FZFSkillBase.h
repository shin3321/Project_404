// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FZFSkillBase.generated.h"

class UAbilitySystemComponent;

UCLASS()
class PROJECT_404_API AFZFSkillBase : public AActor
{
	GENERATED_BODY()

public:
	AFZFSkillBase();

	virtual void InitializeSkill(AActor* InSkillOwner);

protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill")
	float LifeTime = 5.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Skill")
	TObjectPtr<AActor> SkillOwner;

	UPROPERTY(BlueprintReadOnly, Category = "Skill")
	TObjectPtr<UAbilitySystemComponent> SourceASC;

protected:
	UAbilitySystemComponent* GetASCFromActor(AActor* TargetActor) const;

	bool IsServer() const;
};